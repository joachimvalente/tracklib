// projectwidget.h

#ifndef MULTITRACK_PROJECTWIDGET_H
#define MULTITRACK_PROJECTWIDGET_H

#include <QImage>
#include <QPixmap>
#include <QSize>
#include <QStandardItemModel>
#include <QString>
#include <QThread>
#include <QTime>
#include <QWidget>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "abstractplayer.h"
#include "project.h"

namespace Ui {
class ProjectWidget;
}

namespace Multitrack {

class ProjectWidget : public QWidget {
  Q_OBJECT

public:
  explicit ProjectWidget(Project *project, QString location, bool saved,
                         QWidget *parent = 0);
  ~ProjectWidget();

signals:
  void ProjectChanged();
  void ProjectSaved(const QString &location);
  void Closed();

public slots:
  bool Save();  // Return true on success.
  bool SaveAs();  // Return true on success.
  bool Close();

private slots:
  // Player.
  void on_sliderSpeed_valueChanged(int value);
  void on_toolButtonInfo_clicked();
  // time is the time of the request.
  void UpdateFrame(QImage *img, int frame, const QTime &time);
  void OnPlayerStartedPlaying();
  void OnPlayerPaused();
  void OnPlayerCriticalError(const QString &error);
  void SliderMoved();

  void on_toolButtonAddTask_clicked();
  void NewTaskAdded(QObject *o_task);
  void UpdateTaskList();
  void RunTracker(int id);
  void DisplayRunButton(int row);
  void TrackingFinished(int id);
  void OnTrackingError(int id);
  void ShowPreview(int id);

  void MarkProjectChange();  // Indicate that the project changed.

  void on_toolButtonRemoveTask_clicked();
  void ChangeTaskColor(int id);
  void DisplayColorToolButton(int row);

  void on_toolButtonEditTask_clicked();
  void EditTask(int id);

  void on_tableViewTasks_doubleClicked(const QModelIndex &index);

  void on_toolButtonCopyTask_clicked();

  void on_toolButtonExportTask_clicked();
  
private:
  bool DoClose();  // Close without checking if project was saved.

  static QString FrameToTime(int frame, float fps);

  bool eventFilter(QObject *object, QEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void keyReleaseEvent(QKeyEvent *event);

  Ui::ProjectWidget *ui;
  Project *project_;  // Not owned.
  QString location_;
  bool saved_;

  int nb_frames_;

  AbstractPlayer *player_;
  QThread *thread_playback_;
  QTime time_last_updated_;

  QSize img_size_;

  QStandardItemModel *task_model_;
  QThread *thread_tracking_;
  int nb_pending_tasks_;

  QPixmap *untouched_pixmap_;
};

}  // namespace Multitrack

#endif  // MULTITRACKPROJECTWIDGET_H
