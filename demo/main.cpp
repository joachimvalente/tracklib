// main.cpp
//
// Multitrack entry point.
//
// TODO:
//   - UI:
//     * Handle events in ExportDialog
//     * Allow to deselect tasks by clicking outside
//     * Fix the bug where the first task is not highlighted but considered
//       selected
//     * Fix the Color picker behavior
//     * Give all dialogs the ProjectWidget as parent
//   - Code:
//     * Rearrange signals & slots order
//     * Rename the QSignalMapper's
//     * Fix annoying warnings in extern/gsoc11_tracking
//     * Check if there is an easier integration of Tracklib (to shorten the
//       process when adding a new algorithm)
//   - Tracking:
//     * Use one thread per pending task to allow several tasks to run at the
//       same time
//     * Allow to edit non-pending tasks even when some tasks are pending
//     * Allow to cancel a running task
//     * Measure running time of a task
//   - Projects:
//     * Allow to duplicate projects
//   - Exporting:
//     * Export video
//   - Qt:
//     * Fix the "modalSession has been exited prematurely - check for a
//       reentrant call to endModalSession" error when using ComboBox'es
//     * Remove annoying "Reopen windows" message after crashing
//   - Ground truth:
//     * Add a way to manually select ground truth
//     * And add a way to display it
//     * And then add scores to tasks
//   - Platforms:
//     * Deploy on Windows
//
//   - Bugs:
//     * Avoid blocking threads
//     * When video is too heavy to decode playback is too slow and blocks
//     * Default locations on Mac OS X
//     * Weird behavior on Linux (e.g. offset when selecting object)

#include <QApplication>
#include <qglobal.h>
#include <QTime>

#include "mainwindow.h"

Q_DECLARE_METATYPE(Qt::AlignmentFlag)

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QTime time = QTime::currentTime();
  qsrand((uint) time.msec());

  Multitrack::MainWindow w;
  w.show();

  return a.exec();
}
