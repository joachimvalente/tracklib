// projectwidget.cpp

#include "projectwidget.h"
#include "ui_projectwidget.h"

#include <cassert>
#include <cmath>

#include <QCheckBox>
#include <QColorDialog>
#include <QDataStream>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QProgressBar>
#include <QSignalMapper>
#include <QStandardItem>
#include <QStandardPaths>
#include <QTime>
#include <QToolButton>
#include <QVector>
#include <QVBoxLayout>

#include "tracklib.h"

#include "exportdialog.h"
#include "frameplayer.h"
#include "param.h"
#include "projectinfowidget.h"
#include "trackingtask.h"
#include "trackingtaskdialog.h"
#include "videoplayer.h"

namespace Multitrack {

ProjectWidget::ProjectWidget(Project *project, QString location, bool saved,
                             QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ProjectWidget),
  project_(project), location_(location), saved_(saved),
  player_(nullptr), nb_pending_tasks_(0) {
  assert(project != nullptr);

  ui->setupUi(this);

  untouched_pixmap_ = new QPixmap;

  QString error;
  if (!project_->IsValid(&error)) {
    QMessageBox::critical(this, "Error", "Invalid project: " + error);
    ui->groupBoxPlayback->setEnabled(false);
    ui->groupBoxPreview->setEnabled(false);
    ui->groupBoxTracking->setEnabled(false);
    return;
  }

  task_model_ = new QStandardItemModel(0, 7, this);
  task_model_->setHorizontalHeaderItem(0, new QStandardItem(QString("")));
  task_model_->setHorizontalHeaderItem(1, new QStandardItem(QString("Id")));
  task_model_->setHorizontalHeaderItem(2, new QStandardItem(QString("Col.")));
  task_model_->setHorizontalHeaderItem(3, new QStandardItem(QString("Run")));
  task_model_->setHorizontalHeaderItem(
        4, new QStandardItem(QString("Algorithm")));
  task_model_->setHorizontalHeaderItem(
        5, new QStandardItem(QString("First frame")));
  task_model_->setHorizontalHeaderItem(
        6, new QStandardItem(QString("Parameters")));
  task_model_->setHorizontalHeaderItem(7, new QStandardItem(QString("Status")));
  ui->tableViewTasks->setModel(task_model_);
  ui->tableViewTasks->setColumnWidth(0, 25);
  ui->tableViewTasks->setColumnWidth(4, 200);
  ui->tableViewTasks->setColumnWidth(5, 50);
  ui->tableViewTasks->setColumnWidth(6, 70);

  time_last_updated_ = QTime::currentTime();

  if (project_->source_type() == Project::kSourceTypeVideo) {
    player_ = new VideoPlayer(project_->video_path(),
                              project_->first_frame(),
                              project_->last_frame());
    nb_frames_ = project_->last_frame() - project_->first_frame() + 1;
  } else {
    player_ = new FramePlayer(project_->frame_paths(), project_->fps());
    nb_frames_ = project_->frame_paths().count();
  }
  ui->sliderFrame->setMaximum(nb_frames_);

  thread_playback_ = new QThread;
  player_->moveToThread(thread_playback_);
  thread_playback_->start();

  connect(player_, SIGNAL(FrameProcessed(QImage *,int, QTime)),
          this, SLOT(UpdateFrame(QImage *, int, QTime)), Qt::QueuedConnection);
  connect(player_, SIGNAL(CriticalError(QString)),
          this, SLOT(OnPlayerCriticalError(QString)), Qt::QueuedConnection);
  connect(player_, SIGNAL(StartedPlaying()),
          this, SLOT(OnPlayerStartedPlaying()), Qt::QueuedConnection);
  connect(player_, SIGNAL(Paused()),
          this, SLOT(OnPlayerPaused()), Qt::QueuedConnection);
  connect(ui->toolButtonPlay, SIGNAL(clicked()),
          player_, SLOT(TogglePlayPause()), Qt::QueuedConnection);
  connect(ui->toolButtonFirstFrame, SIGNAL(clicked()),
          player_, SLOT(First()), Qt::QueuedConnection);
  connect(ui->toolButtonPreviousFrame, SIGNAL(clicked()),
          player_, SLOT(Previous()), Qt::QueuedConnection);
  connect(ui->toolButtonNextFrame, SIGNAL(clicked()),
          player_, SLOT(Next()), Qt::QueuedConnection);
  connect(ui->sliderFrame, SIGNAL(sliderPressed()),
          player_, SLOT(PauseAlt()), Qt::QueuedConnection);
  connect(ui->sliderFrame, SIGNAL(sliderReleased()),
          player_, SLOT(PlayAlt()), Qt::QueuedConnection);
  connect(ui->sliderFrame, SIGNAL(sliderReleased()),
          this, SLOT(SliderMoved()));

  ui->sliderFrame->installEventFilter(this);
  grabKeyboard();

  QMetaObject::invokeMethod(player_, "Initialize", Qt::QueuedConnection);

  thread_tracking_ = new QThread;
  thread_tracking_->start();
  for (TrackingTask *task : project_->tasks()) {
    task->moveToThread(thread_tracking_);
  }

  UpdateTaskList();

  connect(ui->comboBoxPreview, SIGNAL(currentIndexChanged(int)),
          ui->labelPreview, SLOT(clear()));
}

ProjectWidget::~ProjectWidget() {
  delete ui;
  delete player_;
}

bool ProjectWidget::Save() {
  if (saved_) return true;
  if (location_.isEmpty()) return SaveAs();
  QFile file(location_);
  if (file.open(QIODevice::WriteOnly)) {
    QDataStream ds(&file);
    ds << *project_;
    file.close();
    saved_ = true;
    emit ProjectSaved(location_);
    return true;
  } else {
    QMessageBox::critical(this, "Error", "Could not open " + location_ +
                          " for writing");
    return false;
  }
}

bool ProjectWidget::SaveAs() {
  QString initial_directory =
      (location_.isEmpty()) ?
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) :
        location_;

  location_ = QFileDialog::getSaveFileName(this, "Save as...",
                                           initial_directory,
                                           "Multitrack project (*.mtpro)");
  if (!location_.isEmpty())
    return Save();

  return false;
}

bool ProjectWidget::Close() {
  if (nb_pending_tasks_ > 0) {
    int result = QMessageBox::question(
          this, "Pending tracking tasks",
          "Some trackers are still running. Are you sure?",
          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (result == QMessageBox::No) {
      return false;
    }
  }

  if (!saved_) {
    int result = QMessageBox::question(
          this, "Save changes",
          "Unsaved modifications. Save changes before closing?",
          QMessageBox::Discard | QMessageBox::Save | QMessageBox::Cancel);
    if (result == QMessageBox::Discard) {
      return DoClose();
    } else if (result == QMessageBox::Save) {
      if (Save()) {
        return DoClose();
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else {
    return DoClose();
  }
}

void ProjectWidget::on_sliderSpeed_valueChanged(int value) {
  // Map [0;20] to [0.1x;10x] with 10 corresponding to 1x.
  // i.e. f(value) = 0.1 * 10 ^ (value/10).
  const float map[21] = {0.1f, 0.13f, 0.16f, 0.2f, 0.25f, 0.32f, 0.4f, 0.5f,
                         0.63f, 0.8f, 1.0f, 1.25f, 1.58f, 2.0f, 2.5f, 3.16f,
                         4.0f, 5.0f, 6.3f, 8.0f, 10.0f};
  float speed = map[value];
  ui->labelSpeed->setText(QString::number(speed) + "x");
  QMetaObject::invokeMethod(player_, "ChangeSpeed", Qt::QueuedConnection,
                            Q_ARG(float, speed));
}

void ProjectWidget::on_toolButtonInfo_clicked() {
  (new ProjectInfoWidget(*project_))->show();
}

void ProjectWidget::UpdateFrame(QImage *img, int frame, const QTime &time) {
  assert(img != nullptr);
  // Ensure that we don't show a frame requested in the past.
  if (time > time_last_updated_) {
    time_last_updated_ = time;
    if (!img->isNull()) {
      *untouched_pixmap_ = QPixmap::fromImage(*img);
      QPixmap scaled_pixmap = untouched_pixmap_->scaled(
                                 ui->labelOpencv->size(),
                                 Qt::KeepAspectRatio,
                                 Qt::FastTransformation);
      QSize untouched_size = untouched_pixmap_->size();
      QSize canvas_size = ui->labelOpencv->size();
      QSize scaled_size = untouched_size.scaled(canvas_size,
                                                Qt::KeepAspectRatio);
      for (const TrackingTask *task : project_->tasks()) {
        if (task->active()) {
          QPainter painter(&scaled_pixmap);
          QPen *pen = new QPen(task->color());
          pen->setWidth(3);
          painter.setPen(*pen);

          const QRect &rect = task->result(frame);
          int px = rect.topLeft().x();
          int py = rect.topLeft().y();
          int sx = rect.width();
          int sy = rect.height();
          float ratio = static_cast<float>(untouched_size.width()) /
                        scaled_size.width();
          px = px / ratio;
          py = py / ratio;
          sx /= ratio;
          sy /= ratio;
          QRect rect_scaled(px, py, sx, sy);
          painter.drawRect(rect_scaled);
        }
      }
      ui->labelOpencv->setPixmap(scaled_pixmap);
      img_size_ = img->size();
      delete img;
    }

    ui->sliderFrame->setValue(frame);
    ui->labelFrame->setText(QString::number(frame));
    ui->labelTime->setText(FrameToTime(frame, player_->fps()));
  }
}

void ProjectWidget::OnPlayerCriticalError(const QString &error) {
  QMessageBox::critical(this, "Error", "Player Error: " + error);
  ui->groupBoxPlayback->setEnabled(false);
  ui->groupBoxPreview->setEnabled(false);
  ui->groupBoxTracking->setEnabled(false);
}

void ProjectWidget::SliderMoved() {
  QMetaObject::invokeMethod(player_, "GotoFrame", Qt::QueuedConnection,
                            Q_ARG(int, ui->sliderFrame->value()));
}

void ProjectWidget::OnPlayerStartedPlaying() {
  ui->toolButtonPlay->setIcon(QIcon(":/icons/images/button_pause_blue.png"));
}

void ProjectWidget::OnPlayerPaused() {
  ui->toolButtonPlay->setIcon(QIcon(":/icons/images/button_play_blue.png"));
}

void ProjectWidget::NewTaskAdded(QObject *o_task) {
  TrackingTask *task = static_cast<TrackingTask *>(o_task);
  task->moveToThread(thread_tracking_);
  project_->add_tracking_task(task);
  UpdateTaskList();
  MarkProjectChange();
}

void ProjectWidget::UpdateTaskList() {
  task_model_->removeRows(0, ui->tableViewTasks->model()->rowCount());
  const QVector<TrackingTask *> &tasks = project_->tasks();
  task_model_->setRowCount(tasks.count());
  int row = 0;
  for (TrackingTask *task : tasks) {
    task->set_project_details(
          project_->source_type() == Project::kSourceTypeVideo,
          project_->video_path(), project_->frame_paths());

    QSignalMapper *mapper = new QSignalMapper;

    // Checkbox.
    QCheckBox *active = new QCheckBox;
    active->setEnabled(task->completed());
    active->setChecked(task->active());
    ui->tableViewTasks->setIndexWidget(task_model_->index(row, 0), active);
    connect(active, SIGNAL(clicked(bool)), task, SLOT(set_active(bool)));
    connect(active, SIGNAL(clicked()), this, SLOT(MarkProjectChange()));

    // Id.
    task_model_->setItem(row, 1, new QStandardItem(QString::number(row + 1)));

    // Color.
    DisplayColorToolButton(row);

    // Run.
    DisplayRunButton(row);

    // Algorithm.
    QStandardItem *algo = new QStandardItem(QString(task->algorithm_str()));
    algo->setToolTip(task->algorithm_str());
    task_model_->setItem(row, 4, algo);

    // First frame.
    task_model_->setItem(row, 5, new QStandardItem(
                           QString::number(task->first_frame())));

    // Parameters.
    QToolButton *params = new QToolButton;
    params->setIcon(QIcon(":/icons/images/modify.png"));
    params->setIconSize(QSize(14, 14));
    params->setStyleSheet("border: 0;");
    params->setCursor(Qt::PointingHandCursor);
    ui->tableViewTasks->setIndexWidget(task_model_->index(row, 6), params);
    QSignalMapper *mapper_params = new QSignalMapper;
    connect(params, SIGNAL(clicked()), mapper_params, SLOT(map()));
    mapper_params->setMapping(params, row);
    connect(mapper_params, SIGNAL(mapped(int)), this, SLOT(EditTask(int)));

    // Status.
    QWidget *widget_status = new QWidget;
    QProgressBar *status = new QProgressBar;
    status->setMinimum(0);
    int max = nb_frames_ - task->first_frame() + 1;
    status->setMaximum(max);
    status->setValue(task->completed() ? max : 0);
    status->setAlignment(Qt::AlignVCenter);
    QVBoxLayout *layout_status = new QVBoxLayout(widget_status);
    layout_status->addWidget(status);
    layout_status->setContentsMargins(QMargins(11, 0, 11, 0));
    widget_status->setLayout(layout_status);
    ui->tableViewTasks->setIndexWidget(task_model_->index(row, 7),
                                       widget_status);

    // Connections.
    connect(task, SIGNAL(Processed(int)), status, SLOT(setValue(int)),
            Qt::QueuedConnection);
    connect(task, SIGNAL(Finished()), mapper, SLOT(map()));
    mapper->setMapping(task, row);
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(TrackingFinished(int)),
            Qt::QueuedConnection);
    QSignalMapper *mapper2 = new QSignalMapper;
    connect(task, SIGNAL(Failed()), mapper2, SLOT(map()));
    mapper2->setMapping(task, row);
    connect(mapper2, SIGNAL(mapped(int)), this, SLOT(OnTrackingError(int)),
            Qt::QueuedConnection);
    QSignalMapper *mapper3 = new QSignalMapper;
    connect(task, SIGNAL(Preview()), mapper3, SLOT(map()));
    mapper3->setMapping(task, row);
    connect(mapper3, SIGNAL(mapped(int)), this, SLOT(ShowPreview(int)));

    ++row;
  }
}

void ProjectWidget::RunTracker(int id) {
  if (project_->tasks().at(id)->completed()) {
    int result = QMessageBox::question(
          this, "Task already run",
          "Re-run this task?",
          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (result == QMessageBox::No) {
      return;
    }
  }

  ++nb_pending_tasks_;
  QToolButton *stop = new QToolButton;
  stop->setStyleSheet("border: 0");
  stop->setIcon(QIcon(":/icons/images/wait.png"));
  stop->setIconSize(QSize(18, 18));
  ui->tableViewTasks->setIndexWidget(task_model_->index(id, 3), stop);
  QCheckBox *active = static_cast<QCheckBox *>(
      ui->tableViewTasks->indexWidget(task_model_->index(id, 0)));
  active->setEnabled(false);
  active->setChecked(false);

  QString text_preview = QString::number(id + 1) + " - " +
                         project_->tasks().at(id)->algorithm_str();
  ui->comboBoxPreview->addItem(text_preview);
  ui->comboBoxPreview->setCurrentText(text_preview);
  ui->labelPreview->clear();

  QMetaObject::invokeMethod(project_->tasks().at(id), "Run",
                            Qt::QueuedConnection);
}

void ProjectWidget::DisplayRunButton(int row) {
  QToolButton *run = new QToolButton;
  run->setStyleSheet("border: 0");
  run->setIcon(QIcon(":/icons/images/run.png"));
  run->setIconSize(QSize(18, 18));
  run->setCursor(Qt::PointingHandCursor);
  ui->tableViewTasks->setIndexWidget(task_model_->index(row, 3), run);
  QSignalMapper *mapper = new QSignalMapper;
  connect(run, SIGNAL(clicked()), mapper, SLOT(map()));
  mapper->setMapping(run, row);
  connect(mapper, SIGNAL(mapped(int)), this, SLOT(RunTracker(int)),
          Qt::QueuedConnection);
}

void ProjectWidget::TrackingFinished(int id) {
  DisplayRunButton(id);
  QCheckBox *active = static_cast<QCheckBox *>(
      ui->tableViewTasks->indexWidget(task_model_->index(id, 0)));
  active->setEnabled(true);
  active->setChecked(true);
  MarkProjectChange();
  --nb_pending_tasks_;

  QString text_preview = QString::number(id + 1) + " - " +
                         project_->tasks().at(id)->algorithm_str();
  ui->comboBoxPreview->removeItem(ui->comboBoxPreview->findText(text_preview));
  ui->labelPreview->clear();
}

void ProjectWidget::OnTrackingError(int id) {
  QCheckBox *active = static_cast<QCheckBox *>(
      ui->tableViewTasks->indexWidget(task_model_->index(id, 0)));
  active->setEnabled(false);
  active->setChecked(false);
  static_cast<QProgressBar *>(ui->tableViewTasks->indexWidget(
                                ui->tableViewTasks->model()->index(id, 7)))
      ->setValue(0);
  MarkProjectChange();
  DisplayRunButton(id);
  --nb_pending_tasks_;
  QMessageBox::critical(this, "Error",
                        "Tracking error: " + project_->tasks().at(id)->error());
}

void ProjectWidget::ShowPreview(int id) {
  if (ui->comboBoxPreview->currentText().startsWith(QString::number(id + 1) +
                                                    " -")) {
    QImage img;
    const cv::Mat &preview = project_->tasks().at(id)->preview();
    if (preview.channels() == 3) {
      img = AbstractPlayer::MatColor2QImage(preview);
    } else {
      img = AbstractPlayer::MatGray2QImage(preview);
    }
    ui->labelPreview->setPixmap(QPixmap::fromImage(img).scaled(
                                  ui->labelPreview->size(),
                                  Qt::KeepAspectRatio,
                                  Qt::FastTransformation));
  }
}

void ProjectWidget::MarkProjectChange() {
  if (saved_) {
    saved_ = false;
    emit ProjectChanged();
  }
}

bool ProjectWidget::DoClose() {
  if (player_)
    QMetaObject::invokeMethod(player_, "Pause", Qt::QueuedConnection);

  emit Closed();
  return true;
}

QString ProjectWidget::FrameToTime(int frame, float fps) {
  int time_in_seconds = static_cast<int>(frame / fps);
  int seconds = time_in_seconds % 60;
  int minutes = static_cast<int>(((time_in_seconds / 60) % 60));
  int hours   = static_cast<int>(((time_in_seconds / (3600)) % 24));
  QTime t(hours, minutes, seconds);
  if (hours == 0)
    return t.toString("mm:ss");
  else
    return t.toString("h:mm:ss");
}

bool ProjectWidget::eventFilter(QObject *object, QEvent *event) {
  if (object == ui->sliderFrame && event->type() == QEvent::MouseMove) {
    QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);
    if (mouse_event->buttons() == Qt::LeftButton) {
      SliderMoved();
    }
  }

  return QWidget::eventFilter(object, event);
}

void ProjectWidget::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Left) {
    QMetaObject::invokeMethod(player_, "Previous", Qt::QueuedConnection);
  } else if (event->key() == Qt::Key_Right) {
    QMetaObject::invokeMethod(player_, "Next", Qt::QueuedConnection);
  }
  QWidget::keyPressEvent(event);
}

void ProjectWidget::keyReleaseEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Space) {
    QMetaObject::invokeMethod(player_, "TogglePlayPause",
                              Qt::QueuedConnection);
  }
  QWidget::keyReleaseEvent(event);
}

void ProjectWidget::on_toolButtonAddTask_clicked() {
  if (nb_pending_tasks_ > 0) {
    QMessageBox::critical(this, "Error", "Cannot remove tasks while other "
                          "tasks are pending.");
    return;
  }

  TrackingTask *task = new TrackingTask;
  TrackingTaskDialog *dialog = new TrackingTaskDialog(
                                 untouched_pixmap_,
                                 ui->labelFrame->text().toInt(),
                                 img_size_, task);
  QSignalMapper *mapper = new QSignalMapper;
  connect(dialog, SIGNAL(accepted()), mapper, SLOT(map()));
  mapper->setMapping(dialog, task);
  connect(mapper, SIGNAL(mapped(QObject*)),
          this, SLOT(NewTaskAdded(QObject *)));
  dialog->show();
}

void ProjectWidget::EditTask(int id) {
  if (nb_pending_tasks_ > 0) {
    QMessageBox::critical(this, "Error", "Cannot edit tasks while other "
                                         "tasks are pending.");
    return;
  }

  if (id < 0 || id >= project_->tasks().count()) {
    QMessageBox::critical(this, "Error", "This task does not exist.");
    return;
  }

  TrackingTaskDialog *dialog = new TrackingTaskDialog(
                                 untouched_pixmap_,
                                 ui->labelFrame->text().toInt(),
                                 img_size_, project_->tasks().at(id));
  connect(dialog, SIGNAL(accepted()), this, SLOT(UpdateTaskList()));
  connect(dialog, SIGNAL(accepted()), this, SLOT(MarkProjectChange()));
  dialog->show();
}

void ProjectWidget::on_toolButtonEditTask_clicked() {
  int ind = ui->tableViewTasks->currentIndex().row();
  if (0 <= ind && ind < project_->tasks().count()) {
    EditTask(ind);
  } else {
    QMessageBox::critical(this, "Error", "No task selected.");
    return;
  }
}

void ProjectWidget::on_toolButtonRemoveTask_clicked() {
  int ind = ui->tableViewTasks->currentIndex().row();
  if (0 <= ind && ind < project_->tasks().count()) {
    if (nb_pending_tasks_ == 0) {
      int result = QMessageBox::question(
            this, "Remove tracking task",
            "Are you sure?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
      if (result == QMessageBox::Yes) {
        project_->remove_task(ind);
        MarkProjectChange();
        UpdateTaskList();
      } else {
        return;
      }
    } else {
      QMessageBox::critical(this, "Error", "Cannot remove tasks while other "
                                           "tasks are pending.");
      return;
    }
  } else {
    QMessageBox::critical(this, "Error", "No task selected.");
    return;
  }
}

void ProjectWidget::ChangeTaskColor(int id) {
  TrackingTask *task = project_->tasks().at(id);
  QColorDialog *color_dialog = new QColorDialog(task->color(), this);
  connect(color_dialog, SIGNAL(colorSelected(QColor)),
          task, SLOT(set_color(QColor)));
  connect(color_dialog, SIGNAL(currentColorChanged(QColor)),
          this, SLOT(MarkProjectChange()));
  QSignalMapper *mapper = new QSignalMapper;
  connect(color_dialog, SIGNAL(accepted()), mapper, SLOT(map()));
  connect(color_dialog, SIGNAL(rejected()), mapper, SLOT(map()));
  mapper->setMapping(color_dialog, id);
  connect(mapper, SIGNAL(mapped(int)),
          this, SLOT(DisplayColorToolButton(int)));
  color_dialog->show();
}

void ProjectWidget::DisplayColorToolButton(int row) {
  QWidget *widget_color = new QWidget;
  QToolButton *color = new QToolButton;
  QVBoxLayout *layout_color = new QVBoxLayout(widget_color);
  layout_color->addWidget(color);
  layout_color->setContentsMargins(QMargins(6, 6, 6, 6));
  widget_color->setLayout(layout_color);
  color->setStyleSheet("background: " +
                       project_->tasks().at(row)->color().name() + ";");
  color->setCursor(Qt::PointingHandCursor);
  ui->tableViewTasks->setIndexWidget(task_model_->index(row, 2),
                                     widget_color);
  QSignalMapper *mapper_color = new QSignalMapper;
  connect(color, SIGNAL(clicked()), mapper_color, SLOT(map()));
  mapper_color->setMapping(color, row);
  connect(mapper_color, SIGNAL(mapped(int)),
          this, SLOT(ChangeTaskColor(int)));
}

void ProjectWidget::on_tableViewTasks_doubleClicked(const QModelIndex &index) {
  EditTask(index.row());
}

void ProjectWidget::on_toolButtonCopyTask_clicked() {
  int ind = ui->tableViewTasks->currentIndex().row();
  if (0 <= ind && ind < project_->tasks().count()) {
    if (nb_pending_tasks_ == 0) {
      TrackingTask *task = new TrackingTask(project_->tasks().at(ind));
      task->moveToThread(thread_tracking_);
      project_->add_tracking_task(task);
      UpdateTaskList();
      MarkProjectChange();
    } else {
      QMessageBox::critical(this, "Error", "Cannot add tasks while other "
                                           "tasks are pending.");
      return;
    }
  } else {
    QMessageBox::critical(this, "Error", "No task selected.");
    return;
  }
}

void ProjectWidget::on_toolButtonExportTask_clicked() {
  int ind = ui->tableViewTasks->currentIndex().row();
  if (0 <= ind && ind < project_->tasks().count()) {
    TrackingTask* task = project_->tasks().at(ind);
    if (task->completed()) {
      (new ExportDialog(this, task->results()))->show();
    } else {
      QMessageBox::critical(this, "Error", "Run task before exporting.");
      return;
    }
  } else {
    QMessageBox::critical(this, "Error", "No task selected.");
    return;
  }
}

}  // namespace Multitrack

