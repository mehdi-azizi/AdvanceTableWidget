#include "ItemGenerator.h"
#include "SampleItem.h"

#include <QDateTime>
#include <QThread>
namespace SamplePrj
{
ItemGenerator::ItemGenerator(int suffixId, int itemCount,
                             unsigned long stepInterval, QObject *parent):
  QThread(parent), _state(RS_STOP), _suffixId(suffixId),
  _itemCount(itemCount), _stepInterval(stepInterval)
{
  _icons << QIcon(":/images/led_red.svg")
         << QIcon(":/images/led_yellow.svg")
         << QIcon(":/images/led_green.svg");
}

ItemGenerator::~ItemGenerator()
{
  setParent(nullptr);
}

void  ItemGenerator::generate()
{
  _state = RS_START;

  while (_state != RS_STOP)
  {
    if (_state == RS_START)
    {
      qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));
      auto  r = qrand() % 30;

      if (r < 20)
      {
        addItem();
      }
      else if (r < 25)
      {
        updateItem();
      }
      else if (r < 30)
      {
        deleteItem();
      }

      QThread::msleep(_stepInterval);
    }
    else if (_state == RS_PAUSE)
    {
      QThread::msleep(_stepInterval * 3);
    }
  }

  QThread::exit(0);
}

void  ItemGenerator::pause()
{
  _state = RS_PAUSE;
}

void  ItemGenerator::resume()
{
  _state = RS_START;
}

void  ItemGenerator::stop()
{
  _state = RS_STOP;
}

void  ItemGenerator::addItem(IdType _parent)
{
  qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));
  IdType  id = static_cast<IdType>(_suffixId + _items.count());

  if ((_items.count() >= _itemCount) || (_items.contains(id)))
  {
    if ((!_deleted.isEmpty() && (id != _parent)))
    {
      id = _deleted.takeFirst();
    }
    else
    {
      return;
    }
  }

  auto  it = ItemPtr(new SampleItem(id, QString("Item-%1").arg(id),
                                    false, _icons.first()));

  if (_parent > 0)
  {
    it->setParent(_parent);
  }

  it->updateFromSource(0);
  _items.insert(it->getId(), it);
  changeItem(it, false);

  if (_parent == 0)
  {
    addItem(it->getId());
  }
}

void  ItemGenerator::updateItem()
{
  if (_items.empty()) { return; }

  qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));
  auto  r  = qrand() % _items.count();
  auto  it = _items[r];

  if (!it.isNull())
  {
    it->setName(QString("Item-%1-updated").arg(it->getId()));
    it->setIcon(_icons.at(2));
    it->setState(true);
    _items[r] = it;
    it->updateFromSource(0);

    changeItem(it, false);
  }
}

void  ItemGenerator::deleteItem()
{
  if (_items.empty()) { return; }

  qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));
  auto  r  = static_cast<IdType>(_suffixId + (qrand() % _items.count()));
  auto  it = _items.take(r);

  if (!it.isNull())
  {
    _deleted.append(it->getId());
    changeItem(it, true);
  }
}

void  ItemGenerator::run()
{
  generate();
}
}
