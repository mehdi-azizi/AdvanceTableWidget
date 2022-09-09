#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "custom-table/ACustomTable.h"
#include "custom-table/ATableFilterWidget.h"
#include "custom-table/ATableManager.h"
#include "SampleItem.h"
#include <QTableView>
#include <QBitmap>
#include <QLabel>
#include <QFile>
#include <qsharedpointer.h>
#include <qtimer.h>
#include <QMetaObject>

using namespace  CustomTable;
using namespace  SamplePrj;

MainWindow::MainWindow(QWidget *parent):
  QMainWindow(parent),
  _ui(new Ui::MainWindow), _started(false), _paused(false)
{
  initAppStyle(":/style.qss");
  _ui->setupUi(this);
  initTable();
  initSignals();
  initWaitingWidget();
}

MainWindow::~MainWindow()
{
  stop();
  delete _ui;
}

void  MainWindow::start(int threadCount, int itemPerThread, unsigned long step)
{
  auto  manager = _tableManagerPtr.lock().get();

  if (nullptr != manager)
  {
    changeState(true);

    int  i = 0;

    for (; i < threadCount; i++)
    {
      auto  gen = GeneratorPtr(new SamplePrj::ItemGenerator((i * itemPerThread) + 1,
                                                            itemPerThread, step));
      _generators.append(gen);
      connect(gen.get(), &ItemGenerator::changeItem,
              manager, &ATableManager::changeItem, Qt::ConnectionType::QueuedConnection);
      gen->start();
    }
  }

  _countTimer.start(step);
}

void  MainWindow::pause()
{
  _ui->pb_pause->setText(_paused ? tr("Pause") : tr("Resume"));
  foreach(auto var, _generators)
  {
    if (_paused)
    {
      var->resume();
    }
    else
    {
      var->pause();
    }
  }

  _paused = !_paused;
}

void  MainWindow::stop()
{
  _ui->w_waiting->setVisible(true);
  _ui->progressBar->setMaximum(_generators.count());
  _ui->progressBar->setValue(0);
  _countTimer.stop();
  changeState(false);
  foreach(auto var, _generators)
  {
    var->stop();
    var->wait(1000);
    QApplication::processEvents();
    _ui->progressBar->setValue(_ui->progressBar->value() + 1);
  }
  _generators.clear();

  auto  mang = _tableManagerPtr.lock();

  if (!mang.isNull())
  {
    QTimer::singleShot(100, mang.get(), &ATableManager::clearAll);
  }

  setCountText(true);
  _ui->w_waiting->setVisible(false);
}

void  MainWindow::initAppStyle(const QString &themeFile)
{
  QFile  file(themeFile);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return;
  }

  qApp->setStyleSheet(file.readAll());
}

void  MainWindow::initTable()
{
  auto  _tablePtr     = new ACustomTable(ItemPtr(new SampleItem()), 0);
  auto  _filterWidget = new ATableFilterWidget(_tablePtr);

  _tableManagerPtr = _tablePtr->getManager();

  _ui->widget->layout()->addWidget(_filterWidget);
  _ui->widget->layout()->addWidget(_tablePtr);
}

void  MainWindow::initWaitingWidget()
{
  auto  lay = dynamic_cast<QGridLayout *>(_ui->centralWidget->layout());

  if (nullptr != lay)
  {
    lay->addWidget(_ui->w_waiting, 0, 0, 10, 10);
    _ui->w_waiting->raise();
    _ui->w_waiting->setVisible(false);
  }
}

void  MainWindow::setCountText(bool clear)
{
  auto  mang = _tableManagerPtr.lock();

  if (!mang.isNull())
  {
    _ui->lb_count->setText(QString("%1: %2").arg(tr("Count")).
                           arg(clear ? 0 : mang->getAllItemCount()));
  }
}

void  MainWindow::initSignals()
{
  connect(_ui->pb_start, &QPushButton::clicked, [this](bool)
  {
    if (_started)
    {
      this->stop();
    }
    else
    {
      this->start(_ui->sb_thread_count->value(), _ui->sb_item_count->value(),
                  static_cast<unsigned long>(_ui->sb_step->value()));
    }
  });
  connect(_ui->pb_pause, &QPushButton::clicked, this, &MainWindow::pause);

  connect(&_countTimer, &QTimer::timeout, [this]()
  {
    setCountText();
  });
}

void  MainWindow::changeState(bool start)
{
  _ui->pb_start->setText(start ? tr("Stop") : tr("Start"));
  _ui->pb_pause->setEnabled(start);
  _started = start;
}
