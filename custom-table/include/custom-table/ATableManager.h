#ifndef ATABLEMANAGER_H
#define ATABLEMANAGER_H

#include "ITableItem.h"
#include <QObject>
#include <QMap>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTimer>

#ifdef TABLE_MANAGER_WRITE_LOG
#include <qdebug.h>
#endif

#define TABLE_MANAGER_ID_ROLE  Qt::UserRole

namespace CustomTable
{
class AStandardItem;
class ATableManager: public QStandardItemModel
{
  Q_OBJECT

public:
  ATableManager(quint32 tableId, QObject *p = nullptr);

  ~ATableManager();

  int  getRowCount() const;

  int  getAllItemCount() const;

  QWeakPointer<ITableItem>  getItem(IdType id);

  static int  getIdRole();

  QList<QStandardItem *>  getItemAllColumns(IdType) const;

  quint32  getTableId() const;

  int  getAutoUpdateInterval() const;

  void  setAutoUpdateInterval(int autoUpdateInterval);

public slots:
  void  changeItem(QSharedPointer<ITableItem>, bool isDeleted);

  void  clearAll();

  void  autoUpdateCheck();

private:
  QList<QStandardItem *>  createItems(QSharedPointer<ITableItem> );

  void  updateItem(QSharedPointer<ITableItem> newItem,
                   QSharedPointer<ITableItem> oldItem);

  void  deleteItem(QSharedPointer<ITableItem> );

  void  initInternalSignals();

  QList<IdType>  getAllChildItems(QStandardItem *, bool recursive = true);

private:
  QMap<IdType, QSharedPointer<ITableItem>>  _items;
  QMap<IdType, QList<QStandardItem *>>      _tableItems;
  quint32                                   _tableId;
  qint64                                    _autoUpdateInterval;
  QTimer                                    _autoUpdateTimer;
  qint64                                    _lastAutoUpdate;
};

class AStandardItem: public QStandardItem
{
public:
  ~AStandardItem()
  {
#ifdef TABLE_MANAGER_WRITE_LOG

    if (column() > 1) { return; }

    QString  ret = QString("---deleted Item:%1 Children: ").
                   arg(data(TABLE_MANAGER_ID_ROLE).toLongLong());

    for (int var = 0; var < rowCount(); ++var)
    {
      ret += QString(" %1").arg(child(var, 0)->data(TABLE_MANAGER_ID_ROLE).toLongLong());
    }

    if (nullptr != parent())
    {
      ret += QString(" paretn:%1").arg(parent()->data(TABLE_MANAGER_ID_ROLE).toLongLong());
    }

    qDebug() << ret;
#endif
  }
};
}
#endif // ATABLEMANAGER_H
