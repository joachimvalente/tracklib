// namespace Multitrack

#ifndef MULTITRACK_PROJECTINFOWIDGET_H
#define MULTITRACK_PROJECTINFOWIDGET_H

#include <QWidget>

#include "project.h"

namespace Ui {
class ProjectInfoWidget;
}

namespace Multitrack {

class ProjectInfoWidget : public QWidget {
  Q_OBJECT

public:
  explicit ProjectInfoWidget(const Project &project);
  ~ProjectInfoWidget();

private slots:
  void on_tabWidget_currentChanged(int);

private:
  Ui::ProjectInfoWidget *ui;
  int tabindex_;
};

}  // namespace Multitrack

#endif  // MULTITRACK_PROJECTINFOWIDGET_H

