#include "projectinfowidget.h"
#include "ui_projectinfowidget.h"

namespace Multitrack {

ProjectInfoWidget::ProjectInfoWidget(const Project &project) :
  QWidget(),
  ui(new Ui::ProjectInfoWidget) {
  ui->setupUi(this);

  setFixedSize(size());

  ui->labelName->setText(project.name());
  if (project.source_type() == Project::kSourceTypeVideo) {
    tabindex_ = 0;
    ui->tabWidget->setCurrentIndex(tabindex_);
    ui->lineEditSource->setText(project.video_path());
    ui->lineEditSource->setToolTip(project.video_path());
    ui->labelFramesFromTo->setText("From " +
                                   QString::number(project.first_frame()) +
                                   " to " +
                                   QString::number(project.last_frame()) + ".");
  } else {
    tabindex_ = 1;
    ui->tabWidget->setCurrentIndex(tabindex_);
    ui->listWidgetFrames->addItems(project.frame_paths());
    ui->labelFps->setText("FPS: " + QString::number(project.fps()));
  }
}

ProjectInfoWidget::~ProjectInfoWidget() {
  delete ui;
}

}  // namespace Multitrack

void Multitrack::ProjectInfoWidget::on_tabWidget_currentChanged(int) {
  ui->tabWidget->setCurrentIndex(tabindex_);
}
