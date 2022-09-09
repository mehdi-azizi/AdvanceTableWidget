#include "ATableManager.h"
#include <qdatetime.h>

namespace CustomTable
{
ATableManager::ATableManager(quint32 tableId, QObject *p):
  QStandardItemModel(p),
  _tableId(tableId), _autoUpdateInterval(-1), _lastAutoUpdate(0)
{
  connect(&_autoUpdateTimer, &QTimer::timeout, this, &ATableManager::autoUpdateCheck);
}

ATableManager::~ATableManager()
{
}

int  ATableManager::getRowCount() const
{
  return rowCount();
}

int  ATableManager::getAllItemCount() const
{
  return _items.count();
}

QList<IdType>  ATableManager::getAllChildItems(QStandardItem *a, bool recursive)
{
  QList<IdType>  ret;

  if (nullptr != a)
  {
    for (int var = 0; var < a->rowCount(); ++var)
    {
      auto  x = a->child(var, 0);

      if (nullptr != x)
      {
        ret.append(x->data(TABLE_MANAGER_ID_ROLE).toLongLong());

        if (recursive)
        {
          ret.append(getAllChildItems(x, recursive));
        }
      }
    }
  }

  return ret;
}

int  ATableManager::getAutoUpdateInterval() const
{
  return static_cast<int>(_autoUpdateInterval);
}

void  ATableManager::setAutoUpdateInterval(int autoUpdateInterval)
{
  if ((_autoUpdateInterval <= 0) && (autoUpdateInterval > 0))
  {
    _autoUpdateTimer.start(autoUpdateInterval);
  }
  else if ((_autoUpdateInterval > 0) && (autoUpdateInterval <= 0))
  {
    _autoUpdateTimer.stop();
  }

  _autoUpdateInterval = static_cast<qint64>(autoUpdateInterval);
}

quint32  ATableManager::getTableId() const
{
  return _tableId;
}

QWeakPointer<ITableItem>  ATableManager::getItem(IdType id)
{
  return _items.value(id);
}

int  ATableManager::getIdRole()
{
  return TABLE_MANAGER_ID_ROLE;
}

QList<QStandardItem *>  ATableManager::getItemAllColumns(IdType a) const
{
  return _tableItems.value(a);
}

void  ATableManager::clearAll()
{
  _items.clear();
  _tableItems.clear();
  this->removeRows(0, this->rowCount());
}

void  ATableManager::autoUpdateCheck()
{
  if (QDateTime::currentMSecsSinceEpoch()
      - _lastAutoUpdate >= _autoUpdateInterval)
  {
    int   c   = 0;
    auto  key = _items.keys();
    foreach(auto var, key)
    {
      auto  o = _items.value(var);

      if (!o.isNull() && o->getUpdateNeeded())
      {
        updateItem(o, o);
        o->setUpdateNeeded(false);
        c++;
      }
    }
    qDebug() << QString("=========== %1 item Updated.").arg(c);
    _lastAutoUpdate = QDateTime::currentMSecsSinceEpoch();
  }
}

void  ATableManager::changeItem(QSharedPointer<ITableItem> it, bool isDeleted)
{
  if (it.isNull()) { return; }

  {
    if (!isDeleted)
    {
      auto  old = _items.value(it->getId());

      //= ============== Add new row
      if (old.isNull())
      {
        auto  x = createItems(it);
#ifdef TABLE_MANAGER_WRITE_LOG
        Q_ASSERT(!x.isEmpty());
#endif

        //= =============== is a child
        if (it->hasParent())
        {
          auto  par = _items.value(it->getParent());

          if (!par.isNull())
          {
            auto  ll = _tableItems.value(it->getParent());

            if (ll.size() > 0)
            {
              _tableItems.insert(it->getId(), x);
              _items.insert(it->getId(), it);
              ll.at(0)->appendRow(x);
              par->addChild(it);
#ifdef TABLE_MANAGER_WRITE_LOG
              qDebug() << "$$$$$$$ add1 :" << it->getId() << " to parent:" << it->getParent();
#endif

              return;
            }
          }

          //= ==== can not found the parent
          //= ==== so delete all created Standard Items
          qDeleteAll(x);
        }
        //= =============== is a parent or we cannot find the parent
        else
        {
          _tableItems.insert(it->getId(), x);
          _items.insert(it->getId(), it);
#ifdef TABLE_MANAGER_WRITE_LOG
          qDebug() << "$$$$$$$ add2 :" << it->getId() << " to parent:" << it->getParent();
#endif
          this->invisibleRootItem()->appendRow(x);
        }
      }
      //= =============== update the old one
      else
      {
        updateItem(it, old);
      }
    }
    //= ====================== delete the old one
    else
    {
      deleteItem(it);
    }
  }
}

QList<QStandardItem *>  ATableManager::createItems(QSharedPointer<ITableItem> a)
{
  if (a.isNull()) { return QList<QStandardItem *>(); }

  QList<QStandardItem *>  par;
  int                     j = 0;
  foreach(auto var, a->getAllData(_tableId))
  {
    AStandardItem *i = new AStandardItem();

    i->setData(a->getId(), TABLE_MANAGER_ID_ROLE);

    if (a->isColumnIsCheckBox(j))
    {
      i->setCheckable(true);
      i->setCheckState(var.toBool() ? Qt::Checked : Qt::Unchecked);
      i->setEditable(a->getCheckBoxEditability(j));
    }
    else if ((var.type() == QVariant::Icon)
             || (var.type() == QVariant::Pixmap)
             || (var.type() == QVariant::Image))
    {
      i->setData(var, Qt::DecorationRole);
    }
    else
    {
      i->setData(var, Qt::DisplayRole);
    }

    par.append(i);
    j++;
  }

  if ((a->getChildren().size() > 0) && !par.isEmpty())
  {
    foreach(auto var1, a->getChildren())
    {
      if (!var1.isNull())
      {
        if (!_items.contains(var1->getId()))
        {
          auto  x = createItems(var1);
#ifdef TABLE_MANAGER_WRITE_LOG
          Q_ASSERT(!x.isEmpty());
#endif
          _tableItems.insert(var1->getId(), x);
          _items.insert(var1->getId(), var1);
          par.at(0)->appendRow(x);
#ifdef TABLE_MANAGER_WRITE_LOG
          qDebug() << "$$$$$$$ add3 :" << var1->getId() << " to parent:" << var1->getParent();
#endif
        }
        else
        {
          qWarning() << QString("ATableManager::updateItem <<<"
                                " You want to change the parent. that is wrong (Item:%1)").
            arg(var1->getId());
          deleteItem(_items.value(var1->getId()));
        }
      }
    }
  }

  return par;
}

void  ATableManager::deleteItem(QSharedPointer<ITableItem> a)
{
  if (a.isNull()) { return; }

  auto  ch = _tableItems.take(a->getId());

  if (ch.isEmpty()) { return; }

  auto  children1 = getAllChildItems(ch.at(0));
#ifdef TABLE_MANAGER_WRITE_LOG
  foreach(auto z, a->getChildren())
  {
    if (!children1.contains(z->getId()))
    {
      auto  xIt = _tableItems.value(z->getId());

      if (!xIt.isEmpty())
      {
        auto  xp = xIt.at(0)->parent()->data(TABLE_MANAGER_ID_ROLE).toLongLong();
        qDebug() << "Some child not exist in Table myParent:" << a->getParent()
                 << " tableParent:" << xp;
      }
    }
  }
#endif
  foreach(auto var, children1)
  {
    _items.remove(var);
    _tableItems.remove(var);
  }
  _items.take(a->getId());

  auto  paa        = _tableItems.value(a->getParent());
  auto  parentItem = paa.isEmpty() ? invisibleRootItem() : paa.at(0);

  if (getAllChildItems(parentItem, false).contains(a->getId()))
  {
    parentItem->removeRow(ch.at(0)->row());
  }
  else
  {
    qWarning() << QString("Cannot delete item(%1). Mybe the parent is not right(parent:%2)").
      arg(a->getId()).
      arg(a->getParent());
  }
}

void  ATableManager::updateItem(QSharedPointer<ITableItem> newItem,
                                QSharedPointer<ITableItem> old)
{
  if (newItem.isNull() || old.isNull()) { return; }

  auto  lo = _tableItems.value(newItem->getId());
  auto  ln = newItem->getAllData(_tableId);

  for (int var = 0; var < lo.size() && var < ln.size(); ++var)
  {
    if (newItem->isColumnIsCheckBox(var))
    {
      lo.at(var)->setCheckState(ln.at(var).toBool() ? Qt::Checked : Qt::Unchecked);
    }
    else if ((ln.at(var) == QVariant::Icon)
             || (ln.at(var) == QVariant::Pixmap)
             || (ln.at(var) == QVariant::Image))
    {
      lo.at(var)->setData(ln.at(var), Qt::DecorationRole);
    }
    else
    {
      lo.at(var)->setData(ln.at(var), Qt::DisplayRole);
    }
  }

  if (lo.isEmpty()) { return; }

  if (newItem->isChildrenChanged())
  {
    //= ====== update children
    auto  oo = getAllChildItems(lo.at(0), false);
    auto  nn = newItem->getChildren();
    foreach(auto i, oo)
    {
      auto  var = _items.value(i);

      if (var.isNull()) { continue; }

      bool  is = false;

      for (int var1 = 0; var1 < nn.size(); ++var1)
      {
        if (var->getId() == nn.at(var1)->getId())
        {
          updateItem(nn.at(var1), var);
          nn.removeAt(var1);
          is = true;
          break;
        }
      }

      if (!is)
      {
        deleteItem(var);
      }
    }
    foreach(auto var2, nn)
    {
      if (!_items.contains(var2->getId()))
      {
        auto  x = createItems(var2);
#ifdef TABLE_MANAGER_WRITE_LOG
        Q_ASSERT(!x.isEmpty());
#endif
        _tableItems.insert(var2->getId(), x);
        _items.insert(var2->getId(), var2);
        lo.at(0)->appendRow(x);
#ifdef TABLE_MANAGER_WRITE_LOG
        qDebug() << "$$$$$$$ add4 :" << var2->getId() << " to parent:" << var2->getParent();
#endif
      }
      else
      {
        qWarning() << QString("ATableManager::updateItem <<<"
                              " You want to change the parent. that is wrong (Item:%1)").
          arg(var2->getId());
        deleteItem(_items.value(var2->getId()));
// newItem->removeChild(var2->getId());
      }
    }
  }

  newItem->setChildrenChanged(false);
  _items[newItem->getId()] = newItem;

  // ---------------- send signal to table
  if (!lo.isEmpty())
  {
    dataChanged(lo.first()->index(), lo.last()->index());
  }
}
}
