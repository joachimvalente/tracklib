#ifndef MULTITRACK_WELCOMEWIDGET_H
#define MULTITRACK_WELCOMEWIDGET_H

#include <QWidget>

#include "recentproject.h"

namespace Ui {
class WelcomeWidget;
}

namespace Multitrack {

class WelcomeWidget : public QWidget {
  Q_OBJECT

public:
  explicit WelcomeWidget(QWidget *parent = 0);
  ~WelcomeWidget();

  void UpdateRecentProjects(const QVector<RecentProject> &recent_projects);

signals:
  void New();
  void Open();
  void RecentProjectChosen(int index);

private slots:
  void on_tableWidgetRecent_doubleClicked(const QModelIndex &index);

private:
  Ui::WelcomeWidget *ui;
  QWidget *parent_;
};

}  // namespace Multitrack

#endif  // MULTITRACK_WELCOMEWIDGET_H
