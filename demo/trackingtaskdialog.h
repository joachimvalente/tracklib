// trackingtaskdialog.h
//
// Dialog to choose the tracking algorithm and the object to track.
//
// Note: to add support for a new tracking algorithm,
//   - add it in Tracklib
//   - include it in tracklib.h
//   - in trackingtask.h, add the algorithm among the Algorithm enum
//   - update TrackingTaskDialog's UI and its method accept()
//   - in trackingtask.cpp:
//     * add a using directive
//     * update algorithm_str()
//     * update the switch statement in Run()
//   - update TrackingTaskDialog's constructor when editing.

#ifndef MULTITRACK_TRACKINGTASKDIALOG_H
#define MULTITRACK_TRACKINGTASKDIALOG_H

#include <QDialog>
#include <QPoint>
#include <QSize>

#include <opencv2/core/core.hpp>

#include "trackingtask.h"

namespace Ui {
class TrackingTaskDialog;
}

namespace Multitrack {

class TrackingTaskDialog : public QDialog {
  Q_OBJECT

public:
  explicit TrackingTaskDialog(const QPixmap *pixmap, int frame_number,
                              QSize img_size, TrackingTask *task);
  ~TrackingTaskDialog();

public slots:
  void accept();

private:
  enum MousePos {
    kN,  // On the north edge of the rectangle.
    kNW,  // etc.
    kW,
    kSW,
    kS,
    kSE,
    kE,
    kNE,
    kMiddle,  // Inside the rectangle.
    kNone  // Outside the rectangle.
  };

  bool eventFilter(QObject *object, QEvent *event);

  // Real     = original image reference frame.
  // Relative = pixmap reference frame (scaled from Real).
  // Absolute = labelOpencv reference frame (translated from Relative).
  QPoint AbsoluteToRelative(const QPoint &p) const;
  QPoint RelativeToAbsolute(const QPoint &p) const;
  QPoint AbsoluteToReal(const QPoint &p) const;
  QPoint RealToAbsolute(const QPoint &p) const;

  MousePos WhereIsMouse(const QPoint &p) const;
  void Draw();
  void CanonizePoints();

  Ui::TrackingTaskDialog *ui;
  int frame_number_;
  const QPixmap *original_pixmap_;
  QPixmap *pixmap_;
  QSize size_canvas_;
  QSize img_size_;
  QSize displayed_img_size_;

  // Given in absolute reference frame.
  QPoint start_point_;
  QPoint end_point_;
  bool selecting_;  // Whether user is selecting object.
  bool selected_;  // Whether user has selecting an object.
  MousePos moving_;  // Side being moved.
  QPoint moving_middle_from_;

  TrackingTask *task_;  // Not owned.
};

}  // namespace Multitrack

#endif  // MULTITRACK_TRACKINGTASKDIALOG_H
