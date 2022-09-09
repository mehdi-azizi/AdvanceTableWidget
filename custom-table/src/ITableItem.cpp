#include "ITableItem.h"
#include <qvariant.h>
namespace CustomTable
{
ITableItem::ITableItem(IdType _parent):
  _parent(_parent),
  _id(INVALID_ID_VALUE),
  _isChildrenChanged(false),
  _mutex(new QMutex()),
  _updateNeeded(false)
{
}

ITableItem::~ITableItem()
{
  if (nullptr != _mutex)
  {
    delete _mutex;
    _mutex = nullptr;
  }
}

bool  ITableItem::hasParent() const
{
  return _parent != INVALID_ID_VALUE;
}

IdType  ITableItem::getParent() const
{
  return _parent;
}

bool  ITableItem::hasChild(IdType id) const
{
  QMutexLocker  lock(_mutex);

  foreach(auto var, _children)
  {
    if (var->getId() == id)
    {
      return true;
    }
  }

  return false;
}

QList<QSharedPointer<ITableItem>>  ITableItem::getChildren() const
{
  QMutexLocker  lock(_mutex);

  return _children;
}

void  ITableItem::setParent(const IdType &a)
{
  if (_parent == INVALID_ID_VALUE)
  {
    _parent = a;
  }
}

void  ITableItem::setChildren(const QList<QSharedPointer<ITableItem>> &a)
{
  QMutexLocker  lock(_mutex);

  _isChildrenChanged = true;
  _children          = a;
}

void  ITableItem::addChild(QSharedPointer<ITableItem> a)
{
  QMutexLocker  lock(_mutex);

  foreach(auto var, _children)
  {
    if (var->getId() == a->getId())
    {
      return;
    }
  }
  _isChildrenChanged = true;
  _children.append(a);
}

void  ITableItem::removeChild(const IdType &a)
{
  QMutexLocker  lock(_mutex);

  _isChildrenChanged = true;
  auto  c = _children.count();

  for (int var = c - 1; var >= 0; var--)
  {
    if (_children.at(var)->getId() == a)
    {
      _children.removeAt(var);
    }
  }
}

bool  ITableItem::isChildrenChanged() const
{
  return _isChildrenChanged;
}

void  ITableItem::setChildrenChanged(bool a)
{
  _isChildrenChanged = a;
}

void  ITableItem::setData(quint32 tableId, const QList<QVariant> &a)
{
  QMutexLocker  lock(_mutex);

  if (_data.contains(tableId))
  {
    _data[tableId] = a;
  }
  else
  {
    _data.insert(tableId, a);
  }

  _updateNeeded = true;
}

IdType  ITableItem::getId() const
{
  return _id;
}

void  ITableItem::setId(const IdType &a)
{
  Q_ASSERT(a != INVALID_ID_VALUE);
  _id = a;
}

QList<QVariant>  ITableItem::getAllData(quint32 tableId)
{
  QMutexLocker  lock(_mutex);

  //= === You need to call updateFromSource before this
  Q_ASSERT(!_data.value(tableId).isEmpty());

  return _data.value(tableId);
}

QVariant  ITableItem::getColumnData(quint32 tableId, int a)
{
  QMutexLocker  lock(_mutex);
  auto          x = _data.value(tableId);

  return (a >= 0 && a < x.size()) ? x.at(a) : QVariant();
}

bool  ITableItem::getUpdateNeeded() const
{
  QMutexLocker  lock(_mutex);

  return _updateNeeded || _isChildrenChanged;
}

void  ITableItem::setUpdateNeeded(bool updateNeeded)
{
  QMutexLocker  lock(_mutex);

  _updateNeeded = updateNeeded;
}
}
