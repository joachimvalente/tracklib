// mainwindow.cpp

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QMessageBox>
#include <QSignalMapper>
#include <QStandardPaths>
#include <QTableWidgetItem>

#include "projectwidget.h"
#include "sourceform.h"

namespace Multitrack {

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  project_(new Project),
  is_project_open_(true),
  welcome_size_(425, 437), project_size_(894, 636) {
  ui->setupUi(this);

  // Load recent projects.
  QString data_directory =
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  QString recent_path = QDir::cleanPath(data_directory + QDir::separator() +
                                       "recent.mtcfg");
  QFile recent_file(recent_path);
  if (!recent_file.open(QIODevice::ReadOnly)) {
    if (!recent_file.open(QIODevice::WriteOnly)) {
      qDebug("Could not open file %s for writing.",
             recent_path.toStdString().c_str());
    } else {
      QDataStream ds(&recent_file);
      ds << recent_projects_;
      recent_file.close();
    }
  } else {
    QDataStream ds(&recent_file);
    ds >> recent_projects_;
    recent_file.close();
  }

  ShowWelcome();

  connect(ui->actionNew_project, SIGNAL(triggered()), this, SLOT(NewProject()));
  connect(ui->actionOpen_project, SIGNAL(triggered()),
          this, SLOT(OpenProject()));
  connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::RemoveRecentProject(int pos) {
  recent_projects_.remove(pos);
  UpdateRecentProjects(true);
}

void MainWindow::ShowWelcome() {
  is_project_open_ = false;
  setWindowTitle("Multitrack");

  welcome_widget_ = new WelcomeWidget(this);
  setCentralWidget(welcome_widget_);
  UpdateRecentProjects();
  adjustSize();
  resize(welcome_size_);

  connect(welcome_widget_, SIGNAL(New()), this, SLOT(NewProject()));
  connect(welcome_widget_, SIGNAL(Open()), this, SLOT(OpenProject()));
  connect(welcome_widget_, SIGNAL(RecentProjectChosen(int)),
          this, SLOT(OpenRecentProject(int)));
  ui->actionSave_project->setEnabled(false);
  ui->actionSave_project_as->setEnabled(false);
  ui->actionClose_project->setEnabled(false);
}

void MainWindow::NewProject() {
  if (!is_project_open_ || project_widget_->Close()) {
    SourceForm *source_form = new SourceForm(project_);
    source_form->show();
    connect(source_form, SIGNAL(ProjectCreated()),
            this, SLOT(ViewCreatedProject()));
  }
}

void MainWindow::OpenProject() {
  QString initial_directory =
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  // Workaround for bug on Mac OS.
#if defined(__APPLE__)
  initial_directory += "/dummy.txt";
#endif

  QString filename = QFileDialog::getOpenFileName(
      this, "Open project", initial_directory, "Multitrack project (*.mtpro)");
  if (filename.isEmpty()) return;

  OpenProject(filename);
}

void MainWindow::OpenProject(const QString &filename) {
  if (!is_project_open_ || project_widget_->Close()) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::critical(this, "Error", "Could not open file " + filename);
    } else {
      QDataStream ds(&file);
      ds >> *project_;
      file.close();
      ViewProject(filename, true);

      // Update recent projects.
      RecentProject recent_project(project_->name(), filename);
      int ind = recent_projects_.indexOf(recent_project);
      if (ind != -1) recent_projects_.remove(ind);
      recent_projects_.insert(0, recent_project);
      UpdateRecentProjects(true);
    }
  }
}

void MainWindow::OpenRecentProject(int index) {
  if (0 <= index && index < recent_projects_.count()) {
    const QString &path = recent_projects_.at(index).path();
    OpenProject(path);
  }
}

void MainWindow::ViewCreatedProject() {
  ViewProject("", false);
  ui->actionSave_project->setEnabled(true);
}

void MainWindow::OnProjectChanged() {
  ui->actionSave_project->setEnabled(true);
}

void MainWindow::OnProjectSaved(const QString &location) {
  RecentProject recent_project(project_->name(), location);
  int ind = recent_projects_.indexOf(recent_project);
  if (ind != -1) recent_projects_.remove(ind);
  recent_projects_.insert(0, recent_project);
  UpdateRecentProjects(true);

  ui->actionSave_project->setEnabled(false);
}

void MainWindow::UpdateRecentProjects(bool save) {
  // Limit to 15.
  while (recent_projects_.count() > 15) {
    recent_projects_.remove(15);
  }

  // Populate "Open recent" menu.
  ui->menuOpen_recent->clear();
  QList<QAction *> actions;
  QSignalMapper *mapper = new QSignalMapper;
  for (const RecentProject &recent_project : recent_projects_) {
    QAction *action = new QAction(recent_project.name(), this);
    action->setToolTip(recent_project.path());
    connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
    mapper->setMapping(action, recent_project.path());
    actions.push_back(action);
  }
  connect(mapper, SIGNAL(mapped(QString)), this, SLOT(OpenProject(QString)));
  ui->menuOpen_recent->addActions(actions);
  ui->menuOpen_recent->setEnabled(recent_projects_.count() > 0);

  // Populate "Recent projects" table.
  if (!is_project_open_)
    welcome_widget_->UpdateRecentProjects(recent_projects_);

  // Write to file.
  if (save) {
    QString data_directory =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString recent_path = QDir::cleanPath(data_directory + QDir::separator() +
                                          "recent.mtcfg");
    QFile recent_file(recent_path);
    if (!recent_file.open(QIODevice::WriteOnly)) {
      qDebug("Could not open file %s for writing.",
             recent_path.toStdString().c_str());
    } else {
      QDataStream ds(&recent_file);
      ds << recent_projects_;
      recent_file.close();
    }
  }
}

void MainWindow::ViewProject(const QString &location, bool saved) {
  is_project_open_ = true;
  setWindowTitle("Multitrack - " + project_->name());

  project_widget_ = new ProjectWidget(project_, location, saved, this);
  setCentralWidget(project_widget_);
  adjustSize();
  resize(project_size_);

  ui->actionSave_project_as->setEnabled(true);
  ui->actionClose_project->setEnabled(true);

  connect(ui->actionSave_project_as, SIGNAL(triggered()),
          project_widget_, SLOT(SaveAs()));
  connect(ui->actionSave_project, SIGNAL(triggered()),
          project_widget_, SLOT(Save()));
  connect(ui->actionClose_project, SIGNAL(triggered()),
          project_widget_, SLOT(Close()));

  connect(project_widget_, SIGNAL(Closed()), this, SLOT(ShowWelcome()));
  connect(project_widget_, SIGNAL(ProjectChanged()),
          this, SLOT(OnProjectChanged()));
  connect(project_widget_, SIGNAL(ProjectSaved(QString)),
          this, SLOT(OnProjectSaved(QString)));
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (!is_project_open_ || project_widget_->Close()) {
    QMainWindow::closeEvent(event);
  } else {
    event->ignore();
  }
}


}  // namespace Multitrack
