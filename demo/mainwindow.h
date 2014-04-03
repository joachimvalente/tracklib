// mainwindow.h

#ifndef MULTITRACK_MAINWINDOW_H
#define MULTITRACK_MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QSize>
#include <QVector>

#include "project.h"
#include "projectwidget.h"
#include "recentproject.h"
#include "welcomewidget.h"

namespace Ui {
class MainWindow;
}

namespace Multitrack {

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  virtual ~MainWindow();

public slots:
  void RemoveRecentProject(int pos);

private slots:
  void ShowWelcome();
  void NewProject();  // Create new project.
  void OpenProject();  // Open existing project.
  void OpenProject(const QString &filename);  // Open specified project.
  void OpenRecentProject(int index);  // Open specified project.
  void ViewCreatedProject();  // View newly created project.

  void OnProjectChanged();
  void OnProjectSaved(const QString &location);

private:
  void UpdateRecentProjects(bool save = false);
  void ViewProject(const QString &location, bool saved);  // View project.
  virtual void closeEvent(QCloseEvent *event);

  Ui::MainWindow *ui;
  WelcomeWidget *welcome_widget_;
  ProjectWidget *project_widget_;
  Project *project_;
  QVector<RecentProject> recent_projects_;
  bool is_project_open_;
  const QSize welcome_size_;
  const QSize project_size_;
};

}  // namespace Multitrack

#endif  // MULTITRACK_MAINWINDOW_H
