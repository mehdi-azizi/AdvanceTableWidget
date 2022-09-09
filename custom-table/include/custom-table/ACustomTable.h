#ifndef ACOSTUMTABLE_H
#define ACOSTUMTABLE_H
#include "ITableItem.h"
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <qmenu.h>


namespace CustomTable
{
class ATableManager;
class ASortFilterProxyModel;
class AStandardItemModel;
class ACustomTable: public QTreeView
{
  Q_OBJECT

public:
  ACustomTable(QSharedPointer<ITableItem> sampleData, quint32 tableId,
               QWidget *parent = nullptr);

  ~ACustomTable();

  QWeakPointer<ATableManager>  getManager() const;

  void  setManager(const QSharedPointer<ATableManager> &manager);

  QList<int>  getFilteredColumns() const;

  void  setFilteredColumns(const QList<int> &filteredColumns,
                           bool              matchAllColumns = false);

  void  activeItemsContectMenu(bool);

  QWeakPointer<ITableItem>  getItem(IdType) const;

  int  getRowCount() const;

  int  getAllItemCount() const;

  quint32  getTableId() const;

  void  updateHeaderItems();

  QStringList  getHeaders() const;

  int  getAutoUpdateInterval() const;

  void  setAutoUpdateInterval(int miliSecond);

public slots:
  void  clearTable();

  void  filterRegChanged(const QString &reg);

  void  addItem(QSharedPointer<ITableItem> );

  void  updateItem(QSharedPointer<ITableItem> );

  void  deleteItem(QSharedPointer<ITableItem> );

  // void selectItems(QList<IdType>);
  void  selectItem(IdType);

signals:
  void  itemClicked(QWeakPointer<ITableItem> );

  void  itemDoubleClicked(QWeakPointer<ITableItem> );

  void  itemContextMenuRequested(QWeakPointer<ITableItem>,
                                 const QPoint &globalPos);

  void  itemsSelectionChanged(QList<QWeakPointer<ITableItem>> );

  void  privateInternalSignal(QSharedPointer<ITableItem>, bool isDeleted);

  void  privateClearSignale();

  void  itemCheckStateChanged(QWeakPointer<ITableItem>, bool st, int column);

private slots:
  void  onClicked(const QModelIndex &index);

  void  onDoubleClicked(const QModelIndex &index);

  void  onCustomContextMenuRequested(const QPoint &pos);

  void  onSelectionChanged(const QItemSelection &selected, const QItemSelection&);

private:
  void  initModels();

  void  initHeaders(ITableItem *sampleData);

  void  initHeaderSignal(ITableItem *sampleData);

  void  initInternalSignals();

  QList<QWeakPointer<ITableItem>>  getSelectedItems(const QItemSelection &selected);

private:
  QSharedPointer<ASortFilterProxyModel>  _proxyModel;
  QMenu                                  _headerMenu;
  QSharedPointer<ITableItem>             _sampleData;
  QSharedPointer<ATableManager>          _manager;
  QList<int>                             _filteredColumns;
  quint32                                _tableId;
  QList<IdType>                          _selected;
};

class ASortFilterProxyModel: public QSortFilterProxyModel
{
public:
  QList<int>  filteredColumns() const;

  void  setFilteredColumns(const QList<int> &filteredColumns, bool matchAllColumns = false);

private:
  QList<int>  _filteredColumns;
  bool        _corespond = false;

protected:
  bool  filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};
}


#endif // ACOSTUMTABLE_H
