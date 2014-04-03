// exportdialog.cpp

#include "exportdialog.h"
#include "ui_exportdialog.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>

namespace Multitrack {

ExportDialog::ExportDialog(QWidget *parent, const QVector<QRect>& results) :
  QDialog(parent),
  ui(new Ui::ExportDialog),
  results_(results) {
  ui->setupUi(this);

  setFixedSize(size());

  installEventFilter(ui->labelElementLine);
  installEventFilter(ui->labelElementSpace);
  installEventFilter(ui->labelFrameLine);
  installEventFilter(ui->labelFrameSpace);
  connect(ui->comboBoxElementSep, SIGNAL(highlighted(int)),
          ui->radioButtonElementChar, SLOT(click()));
  connect(ui->comboBoxFrameSep, SIGNAL(highlighted(int)),
          ui->radioButtonFrameChar, SLOT(click()));
}

ExportDialog::~ExportDialog() {
  delete ui;
}

void ExportDialog::accept() {
  QString filename = QFileDialog::getSaveFileName(
      this, "Export to file...",
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

  if (!filename.isEmpty()) {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream file_stream(&file);

      QString frame_sep;
      QString element_sep;
      if (ui->radioButtonFrameChar->isChecked()) {
        frame_sep = ui->comboBoxFrameSep->currentText();
      } else if (ui->radioButtonFrameSpace->isChecked()) {
        frame_sep = " ";
      } else if (ui->radioButtonFrameLine->isChecked()) {
        frame_sep = "\n";
      }
      if (ui->radioButtonElementChar->isChecked()) {
        element_sep = ui->comboBoxElementSep->currentText();
      } else if (ui->radioButtonElementSpace->isChecked()) {
        element_sep = " ";
      } else if (ui->radioButtonElementLine->isChecked()) {
        element_sep = "\n";
      }

      bool first = true;
      for (const QRect &rect : results_) {
        if (first) first = false;
        else file_stream << frame_sep;

        if (ui->comboBoxFormat->currentIndex() == 0) {
          // Center, Width, Height.
          file_stream << rect.center().x() << element_sep <<
                         rect.center().y() << element_sep <<
                         rect.width() << element_sep <<
                         rect.height();
        } else if (ui->comboBoxFormat->currentIndex() == 1) {
          // TopLeft, BottomRight.
          file_stream << rect.topLeft().x() << element_sep <<
                         rect.topLeft().y() << element_sep <<
                         rect.bottomRight().x() << element_sep <<
                         rect.bottomRight().y();
        } else if (ui->comboBoxFormat->currentIndex() == 2) {
          // TopLeft, Width, Height.
          file_stream << rect.topLeft().x() << element_sep <<
                         rect.topLeft().y() << element_sep <<
                         rect.width() << element_sep <<
                         rect.height();
        }
      }

      file.close();
      QMessageBox::information(this, "", "Results exported to " + filename +
                                         ".");
      QDialog::accept();
    } else {
      QMessageBox::critical(this, "Error", "Could not open " + filename +
                                           " for writing.");
      return;
    }
  }
}

bool ExportDialog::eventFilter(QObject *object, QEvent *event) {
  if (event->type() == QEvent::MouseButtonRelease) {
    qDebug("here");
    if (object == ui->labelElementLine) {
      ui->radioButtonElementLine->click();
      return true;
    }
    else if (object == ui->labelElementSpace) {
      ui->radioButtonElementSpace->click();
      return true;
    }
    else if (object == ui->labelFrameLine) {
      ui->radioButtonFrameLine->click();
      return true;
    }
    else if (object == ui->labelFrameSpace) {
      ui->radioButtonFrameSpace->click();
      return true;
    }
  }
  return QDialog::eventFilter(object, event);
}

}  // namespace Multitrack
