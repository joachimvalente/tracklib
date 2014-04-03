// sourceform.h

#ifndef MULTITRACK_SOURCEFORM_H
#define MULTITRACK_SOURCEFORM_H

#include <QStringList>
#include <QWidget>

#include "project.h"

namespace Ui {
class SourceForm;
}

namespace Multitrack {

class SourceForm : public QWidget {
  Q_OBJECT

public:
  explicit SourceForm(Project *project, QWidget *parent = 0);
  virtual ~SourceForm();

signals:
  void ProjectCreated();

private slots:
  void EnableSelection(bool enable);

  void on_comboBoxType_currentIndexChanged(int index);
  void on_toolButtonSource_clicked();
  void on_toolButtonAdd_clicked();
  void on_toolButtonRemove_clicked();
  void on_buttonBox_accepted();

private:
  void UpdateFramePaths();
  int FrameToSeconds(int frame) const;
  int SecondsToFrame(int seconds) const;

  Ui::SourceForm *ui;
  QList<QWidget *> selection_widgets_;
  Project *project_;  // Not owned.
  QStringList frame_paths_;
  int nb_frames_;
  float fps_;
};

}  // namespace Multitrack

#endif  // MULTITRACK_SOURCEFORM_H
