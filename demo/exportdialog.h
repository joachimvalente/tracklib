// exportdialog.h

#ifndef MULTITRACK_EXPORTDIALOG_H
#define MULTITRACK_EXPORTDIALOG_H

#include <QDialog>
#include <QRect>
#include <QVector>

namespace Ui {
class ExportDialog;
}

namespace Multitrack {

class ExportDialog : public QDialog {
  Q_OBJECT

public:
  explicit ExportDialog(QWidget *parent, const QVector<QRect> &results);
  ~ExportDialog();

private:
  void accept();
  bool eventFilter(QObject *object, QEvent *event);

  Ui::ExportDialog *ui;
  const QVector<QRect> &results_;
};

}  // namespace Multitrack

#endif  // MULTITRACK_EXPORTDIALOG_H
