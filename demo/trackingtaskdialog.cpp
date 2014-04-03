// trackingtaskdialog.cpp

#include "trackingtaskdialog.h"
#include "ui_trackingtaskdialog.h"

#include <cassert>
#include <cmath>

#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QRect>

#include "param.h"

namespace Multitrack {

TrackingTaskDialog::TrackingTaskDialog(const QPixmap *pixmap,
                                       int frame_number, QSize img_size,
                                       TrackingTask *task) :
  QDialog(),
  ui(new Ui::TrackingTaskDialog),
  frame_number_(frame_number),
  original_pixmap_(pixmap),
  img_size_(img_size),
  selecting_(false),
  selected_(false),
  moving_(kNone),
  task_(task) {
  assert(pixmap != nullptr);
  assert(task != nullptr);

  ui->setupUi(this);

  setFixedSize(size());

  // /!\ Problem here with getting the correct size dynamically.
  size_canvas_ = QSize(593, 359);

  displayed_img_size_ = img_size_.scaled(size_canvas_, Qt::KeepAspectRatio);

  ui->labelFrame->setText("Frame " + QString::number(frame_number_));
  original_pixmap_ = new QPixmap(pixmap->scaled(size_canvas_,
                                       Qt::KeepAspectRatio,
                                       Qt::FastTransformation));
  ui->labelOpencv->setPixmap(*original_pixmap_);

  ui->labelOpencv->installEventFilter(this);

  // Update UI values if editing and not creating.
  if (!task_->object().isEmpty()) {
    ui->tabWidget->setCurrentIndex(1);

    // Object selection.
    start_point_ = RealToAbsolute(task_->object().topLeft());
    end_point_ = RealToAbsolute(task_->object().bottomRight());
    selected_ = true;
    Draw();

    // Algorithm selection.
    ui->tabWidgetAlgo->setCurrentIndex(static_cast<int>(task_->algorithm()));
    if (task_->algorithm() == TrackingTask::kTemplateMatching) {
      // Template matching.
      ui->comboBoxTmMethod->setCurrentIndex(task_->param(0).GetI());
    } else if (task_->algorithm() == TrackingTask::kMeanshift) {
      // Meanshift.
      ui->comboBoxMeanshiftVariant->setCurrentIndex(task_->param(0).GetI());
      ui->comboBoxMeanshiftChannels->setCurrentIndex(task_->param(1).GetI());
      ui->spinBoxMeanshiftIter->setValue(task_->param(2).GetI());
    }

    // Filter.
    ui->tabWidgetFilter->setCurrentIndex(static_cast<int>(task_->filter()));
    if (task_->filter() == TrackingTask::kKalmanFilter) {
      ui->doubleSpinBoxKalmanQ->setValue(task_->filter_param(0).GetF());
      ui->doubleSpinBoxKalmanR->setValue(task_->filter_param(1).GetF());
    }

    // Bgs.
    if (task_->bgs() != TrackingTask::kNoBgs) {
      ui->groupBoxBgs->setChecked(true);
      ui->comboBoxBgsMethod->setCurrentIndex(task_->bgs_param(0).GetI());
    }
  } else {
    task_->set_first_frame(frame_number);
  }
}

TrackingTaskDialog::~TrackingTaskDialog() {
  delete ui;
}

bool TrackingTaskDialog::eventFilter(QObject *object, QEvent *event) {
  // Button press.
  if (object == ui->labelOpencv &&
        event->type() == QEvent::MouseButtonPress) {
    QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);

    // Start selection.
    if (!selected_ || WhereIsMouse(mouse_event->pos()) == kNone) {
      start_point_ = QPoint(mouse_event->pos());
      selecting_ = true;
    }

    // Start resizing.
    else if (selected_) {
      moving_ = WhereIsMouse(mouse_event->pos());
      if (moving_ == kMiddle) moving_middle_from_ = mouse_event->pos();
    }
  }

  // Mouse move.
  else if (object == ui->labelOpencv &&
      event->type() == QEvent::MouseMove) {
    QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);

    // Continue selection.
    if (selecting_) {
      end_point_ = QPoint(mouse_event->pos());
      Draw();
    }

    // Continue resizing or update cursor.
    else if (selected_) {
      MousePos mpos = WhereIsMouse(mouse_event->pos());
      if (moving_ == kNone) {
        if (mpos == kN || mpos == kS)
          ui->labelOpencv->setCursor(Qt::SizeVerCursor);
        else if (mpos == kE || mpos == kW)
          ui->labelOpencv->setCursor(Qt::SizeHorCursor);
        else if (mpos == kNW || mpos == kSE)
          ui->labelOpencv->setCursor(Qt::SizeFDiagCursor);
        else if (mpos == kNE || mpos == kSW)
          ui->labelOpencv->setCursor(Qt::SizeBDiagCursor);
        else if (mpos == kMiddle)
          ui->labelOpencv->setCursor(Qt::SizeAllCursor);
        else ui->labelOpencv->setCursor(Qt::CrossCursor);
      }

      if (moving_ == kN || moving_ == kNE || moving_ == kNW)
        start_point_.setY(mouse_event->pos().y());
      if (moving_ == kS || moving_ == kSE || moving_ == kSW)
        end_point_.setY(mouse_event->pos().y());
      if (moving_ == kE || moving_ == kNE || moving_ == kSE)
        end_point_.setX(mouse_event->pos().x());
      if (moving_ == kW || moving_ == kNW || moving_ == kSW)
        start_point_.setX(mouse_event->pos().x());
      if (moving_ == kMiddle) {
        start_point_.setX(start_point_.x() +
                          (mouse_event->pos().x() - moving_middle_from_.x()));
        start_point_.setY(start_point_.y() +
                          (mouse_event->pos().y() - moving_middle_from_.y()));
        end_point_.setX(end_point_.x() +
                          (mouse_event->pos().x() - moving_middle_from_.x()));
        end_point_.setY(end_point_.y() +
                          (mouse_event->pos().y() - moving_middle_from_.y()));
        moving_middle_from_ = QPoint(mouse_event->pos());
      }
      if (moving_ != kNone) {
        Draw();
      }
    }
  }

  // Button release.
  else if (event->type() == QEvent::MouseButtonRelease) {

    // Stop selection.
    if (selecting_) {
      selecting_ = false;
      selected_ = true;
      CanonizePoints();
      Draw();
    }

    // Stop resizing.
    else if (moving_ != kNone) {
      CanonizePoints();
      Draw();
      moving_ = kNone;
    }
  }

  return QDialog::eventFilter(object, event);
}

QPoint TrackingTaskDialog::AbsoluteToRelative(const QPoint &p) const {
  QPoint q;
  q.setX(p.x() - (size_canvas_.width() - displayed_img_size_.width()) / 2);
  q.setY(p.y() - (size_canvas_.height() - displayed_img_size_.height()) / 2);
  return q;
}

QPoint TrackingTaskDialog::RelativeToAbsolute(const QPoint &p) const {
  QPoint q;
  q.setX(p.x() + (size_canvas_.width() - displayed_img_size_.width()) / 2);
  q.setY(p.y() + (size_canvas_.height() - displayed_img_size_.height()) / 2);
  return q;
}

QPoint TrackingTaskDialog::AbsoluteToReal(const QPoint &p) const {
  return AbsoluteToRelative(p) *
      static_cast<float>(img_size_.width()) / displayed_img_size_.width();
}

QPoint TrackingTaskDialog::RealToAbsolute(const QPoint &p) const {
  return RelativeToAbsolute(p /
      (static_cast<float>(img_size_.width()) / displayed_img_size_.width()));
}

TrackingTaskDialog::MousePos TrackingTaskDialog::WhereIsMouse(const QPoint &p)
const {
  if (start_point_.x() - 5 < p.x() && p.x() < start_point_.x() + 5 &&
      start_point_.y() - 5 < p.y() && p.y() < start_point_.y() + 5) {
    return kNW;
  } else if (start_point_.x() - 5 < p.x() && p.x() < start_point_.x() + 5 &&
             end_point_.y() - 5 < p.y() && p.y() < end_point_.y() + 5) {
    return kSW;
  } else if (end_point_.x() - 5 < p.x() && p.x() < end_point_.x() + 5 &&
             end_point_.y() - 5 < p.y() && p.y() < end_point_.y() + 5) {
    return kSE;
  } else if (end_point_.x() - 5 < p.x() && p.x() < end_point_.x() + 5 &&
             start_point_.y() - 5 < p.y() && p.y() < start_point_.y() + 5) {
    return kNE;
  } else if (start_point_.x() - 5 < p.x() && p.x() < start_point_.x() + 5 &&
             start_point_.y() + 5 < p.y() && p.y() < end_point_.y() - 5) {
    return kW;
  } else if (end_point_.x() - 5 < p.x() && p.x() < end_point_.x() + 5 &&
             start_point_.y() + 5 < p.y() && p.y() < end_point_.y() - 5) {
    return kE;
  } else if (start_point_.x() + 5 < p.x() && p.x() < end_point_.x() - 5 &&
             start_point_.y() - 5 < p.y() && p.y() < start_point_.y() + 5) {
    return kN;
  } else if (start_point_.x() + 5 < p.x() && p.x() < end_point_.x() - 5 &&
             end_point_.y() - 5 < p.y() && p.y() < end_point_.y() + 5) {
    return kS;
  } else if (start_point_.x() + 5 < p.x() && p.x() < end_point_.x() - 5 &&
             start_point_.y() + 5 < p.y() && p.y() < end_point_.y() - 5) {
    return kMiddle;
  }
  return kNone;
}

void TrackingTaskDialog::Draw() {
  pixmap_ = new QPixmap(*original_pixmap_);
  QPainter painter(pixmap_);
  painter.setPen(*(new QColor(255, 0, 255, 255)));
  QRect rect;
  rect.setTopLeft(AbsoluteToRelative(start_point_));
  rect.setBottomRight(AbsoluteToRelative(end_point_));
  painter.drawRect(rect);
  ui->labelOpencv->setPixmap(*pixmap_);
}

void TrackingTaskDialog::CanonizePoints() {
  // Set start_point_ to the top left, end_point_ to the bottom right.
  QPoint top_left_(std::min(start_point_.x(), end_point_.x()),
                   std::min(start_point_.y(), end_point_.y()));
  end_point_ = QPoint(std::max(start_point_.x(), end_point_.x()),
                      std::max(start_point_.y(), end_point_.y()));
  start_point_ = top_left_;

  QPoint max_point = RelativeToAbsolute(QPoint(
                                          displayed_img_size_.width() - 2,
                                          displayed_img_size_.height() - 2));
  QPoint min_point = RelativeToAbsolute(QPoint(0 ,0));

  end_point_.setX(std::max(min_point.x(),
                           std::min(max_point.x(), end_point_.x())));
  end_point_.setY(std::max(min_point.y(),
                           std::min(max_point.y(), end_point_.y())));
  start_point_.setX(std::max(min_point.x(),
                           std::min(max_point.x(), start_point_.x())));
  start_point_.setY(std::max(min_point.y(),
                           std::min(max_point.y(), start_point_.y())));
}

void TrackingTaskDialog::accept() {
  if (!selected_) {
    ui->tabWidget->setCurrentIndex(0);
    QMessageBox::critical(this, "Error", "No object selected.");
    return;
  }

  if (QRect(start_point_, end_point_).isEmpty()) {
    ui->tabWidget->setCurrentIndex(0);
    QMessageBox::critical(this, "Error", "Object selected is empty.");
    return;
  }

  cv::Rect object;
  QPoint start = AbsoluteToReal(start_point_);
  QPoint end = AbsoluteToReal(end_point_);
  object.x = start.x();
  object.y = start.y();
  object.width = end.x() - start.x();
  object.height = end.y() - start.y();
  task_->set_object(object);

  int algo = ui->tabWidgetAlgo->currentIndex();

  // Template matching.
  if (algo == 0) {
    QVector<Param> params;
    params.push_back(Param(ui->comboBoxTmMethod->currentIndex()));
    task_->set_tracker(TrackingTask::kTemplateMatching, params);
  }

  // Meanshift.
  else if (algo == 1) {
    QVector<Param> params;
    params.push_back(Param(ui->comboBoxMeanshiftVariant->currentIndex()));
    params.push_back(Param(ui->comboBoxMeanshiftChannels->currentIndex()));
    params.push_back(Param(ui->spinBoxMeanshiftIter->value()));
    task_->set_tracker(TrackingTask::kMeanshift, params);
  }

  // Filter.
  int filter = ui->tabWidgetFilter->currentIndex();

  if (filter == 0) {
    QVector<Param> filter_params;
    task_->set_filter(TrackingTask::kNoFilter, filter_params);
  } else if (filter == 1) {
    QVector<Param> filter_params;
    filter_params.push_back(Param(ui->doubleSpinBoxKalmanQ->value()));
    filter_params.push_back(Param(ui->doubleSpinBoxKalmanR->value()));
    task_->set_filter(TrackingTask::kKalmanFilter, filter_params);
  }

  // Bgs.
  bool bgs = (ui->groupBoxBgs->isChecked());

  if (bgs) {
    QVector<Param> bgs_params;
    bgs_params.push_back(Param(ui->comboBoxBgsMethod->currentIndex()));
    task_->set_bgs(TrackingTask::kOnlineBgs, bgs_params);
  } else {
    QVector<Param> bgs_params;
    task_->set_bgs(TrackingTask::kNoBgs, bgs_params);
  }

  task_->ClearResults();
  QDialog::accept();
}

}  // namespace Multitrack
