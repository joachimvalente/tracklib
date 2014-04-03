// sourceform.cpp

#include "sourceform.h"
#include "ui_sourceform.h"

#include <cassert>

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QModelIndex>
#include <QStandardPaths>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Multitrack {

SourceForm::SourceForm(Project *project, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::SourceForm),
  project_(project), nb_frames_(1), fps_(1.0f) {
  assert(project != nullptr);

  ui->setupUi(this);
  project_->clear_tasks();

  setFixedSize(size());  // Prevent resizing.

  selection_widgets_ << ui->comboBoxType << ui->spinBoxFrom << ui->labelTo <<
                        ui->spinBoxTo;
  for (QWidget *widget : selection_widgets_) widget->setEnabled(false);

  connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
  connect(ui->radioButtonSelection, SIGNAL(toggled(bool)),
          this, SLOT(EnableSelection(bool)));
}

SourceForm::~SourceForm() {
  delete ui;
}

void SourceForm::EnableSelection(bool enable) {
  for (QWidget *widget : selection_widgets_)
    widget->setEnabled(enable);
}

void SourceForm::on_comboBoxType_currentIndexChanged(int index) {
  if (index == 0) {
    // Frames.
    ui->spinBoxFrom->setMinimum(1);
    ui->spinBoxTo->setMinimum(1);
    ui->spinBoxFrom->setMaximum(nb_frames_);
    ui->spinBoxTo->setMaximum(nb_frames_);
    ui->spinBoxFrom->setValue(1);
    ui->spinBoxTo->setValue(nb_frames_);
  } else {
    // Seconds.
    int max = FrameToSeconds(nb_frames_);
    ui->spinBoxFrom->setMinimum(0);
    ui->spinBoxTo->setMinimum(0);
    ui->spinBoxFrom->setMaximum(max);
    ui->spinBoxTo->setMaximum(max);
    ui->spinBoxFrom->setValue(0);
    ui->spinBoxTo->setValue(max);
  }
}

void SourceForm::on_toolButtonSource_clicked() {
  QString initial_directory;
  if (ui->lineEditSource->text().isEmpty()) {
    initial_directory =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        // Workaround for bug on Mac OS.
#if defined(__APPLE__)
        initial_directory += "/dummy.txt";
#endif
  } else {
    ui->lineEditName->text();
  }

  QString filename = QFileDialog::getOpenFileName(
      this, "Select video file", initial_directory,
      "Videos (*.3gp *.aaf *asf *.avi *.flv *.m4v *.mkv *.mov *.mp4 *.mpeg "
      "*.mpg *.ogg *.swf *.wmv)");
  if (filename.isEmpty()) return;
  if (filename == ui->lineEditSource->text()) return;

  // Try to read the video and retrieve the number of frames.
  cv::VideoCapture cap(filename.toStdString());
  if (!cap.isOpened()) {
    QMessageBox::critical(this, "Error", "Could not read video " + filename);
    return;
  }

  nb_frames_ = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_COUNT));
  fps_ = static_cast<float>(cap.get(CV_CAP_PROP_FPS));
  if (nb_frames_ == 0) {
    QMessageBox::critical(this, "Error", "Video is empty.");
    return;
  }

  ui->lineEditSource->setText(filename);

  // Update remaining parameters.
  ui->radioButtonAll->setChecked(true);
  ui->radioButtonAll->setText("All " + QString::number(nb_frames_) + " frames");
  ui->comboBoxType->setCurrentIndex(0);
  ui->spinBoxFrom->setMinimum(1);
  ui->spinBoxTo->setMinimum(1);
  ui->spinBoxFrom->setMaximum(nb_frames_);
  ui->spinBoxTo->setMaximum(nb_frames_);
  ui->spinBoxFrom->setValue(1);
  ui->spinBoxTo->setValue(nb_frames_);
}

void SourceForm::on_toolButtonAdd_clicked() {
  QString initial_directory =
      (frame_paths_.isEmpty()) ?
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) :
        QFileInfo(frame_paths_.back()).absolutePath();

  // Workaround for bug on Mac OS.
#if defined(__APPLE__)
  initial_directory += "/dummy.txt";
#endif

  QStringList frame_paths = QFileDialog::getOpenFileNames(
      this, "Select frames", initial_directory,
      "Supported image files (*.bmp *.dib *.jp2 *.jpe *.jpeg *.jpg *.pbm "
      "*.pgm *.png *.ppm *.ras *.sr *.tif *.tiff);;"
      "Windows bitmaps (*.bmp *.dib);;JPEG files (*.jpeg *.jpg *.jpe);;"
      "JPEG 2000 files (*.jp2);;Portable Network Graphics (*.png);;"
      "Portable image format (*.pbm *.pgm *.ppm);;Sun rasters (*.sr *.ras);;"
      "TIFF files (*.tiff *.tif)");
  if (frame_paths.count() == 0) return;

  if (ui->listWidgetFrames->selectionModel()->selectedIndexes().count() == 1) {
    int pos =
        ui->listWidgetFrames->selectionModel()->selectedIndexes().at(0).row();
    for (const QString &path : frame_paths) {
      frame_paths_.insert(++pos, path);
    }
  } else {
    for (const QString &path : frame_paths) {
      frame_paths_.push_back(path);
    }
  }

  UpdateFramePaths();
}

void SourceForm::on_toolButtonRemove_clicked() {
  if (ui->listWidgetFrames->selectionModel()->selectedIndexes().count() == 0)
    return;

  for (const QModelIndex &index :
       ui->listWidgetFrames->selectionModel()->selectedIndexes()) {
    frame_paths_.replace(index.row(), "");
  }

  frame_paths_.removeAll("");
  UpdateFramePaths();
}

void SourceForm::on_buttonBox_accepted() {
  project_->set_name(ui->lineEditName->text());
  int c = ui->tabWidgetSource->currentIndex();
  if (c == 0) {
    project_->set_source_type(Project::kSourceTypeVideo);
    project_->set_video_path(ui->lineEditSource->text());
    if (ui->comboBoxType->currentIndex() == 0) {
      project_->set_first_frame(ui->spinBoxFrom->value());
      project_->set_last_frame(ui->spinBoxTo->value());
    } else {
      project_->set_first_frame(SecondsToFrame(ui->spinBoxFrom->value()));
      project_->set_last_frame(SecondsToFrame(ui->spinBoxTo->value()));
    }
  } else {
    project_->set_source_type(Project::kSourceTypeFrames);
    project_->set_frame_paths(frame_paths_);
    project_->set_fps(ui->doubleSpinBoxFps->value());
  }

  QString error;
  if (project_->IsValid(&error)) {
    emit ProjectCreated();
    close();
  } else {
    QMessageBox::critical(this, "Error", error);
  }
}

void SourceForm::UpdateFramePaths() {
  ui->listWidgetFrames->clear();
  ui->listWidgetFrames->addItems(frame_paths_);
}

int SourceForm::SecondsToFrame(int seconds) const {
  return seconds * fps_ + 1;
}

int SourceForm::FrameToSeconds(int frame) const {
  return static_cast<int>((frame - 1) / fps_);
}

}  // namespace Multitrack
