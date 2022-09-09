#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ItemGenerator.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <memory>
#include <qtimer.h>
namespace Ui
{
class MainWindow;
}
namespace CustomTable
{
class ATableManager;
}
using namespace CustomTable;


class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);

  ~MainWindow();

private slots:
  void  start(int threadCount, int itemPerThread, unsigned long step);

  void  pause();

  void  stop();

private:
  void  initAppStyle(const QString &themeFile);

  void  initTable();

  void  initWaitingWidget();

  void  initSignals();

  void  changeState(bool start);

private:
  Ui::MainWindow                 *_ui;
  bool                            _started;
  bool                            _paused;
  QWeakPointer<ATableManager>     _tableManagerPtr;
  QList<SamplePrj::GeneratorPtr>  _generators;
  QTimer                          _countTimer;
  QWidget                        *_waitingWidget;

  void  setCountText(bool clear = false);
};

#endif // MAINWINDOW_H
