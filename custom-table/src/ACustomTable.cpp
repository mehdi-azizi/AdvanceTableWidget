#include "ACustomTable.h"
#include "ATableManager.h"
#include <qheaderview.h>
#include <QStandardItemModel>
#include <qregularexpression.h>
namespace CustomTable
{
ACustomTable::ACustomTable(QSharedPointer<ITableItem> sampleData, quint32 tableId,
                           QWidget *parent):
  QTreeView(parent),
  _sampleData(sampleData), _tableId(tableId)
{
  Q_ASSERT(!sampleData.isNull());

  qRegisterMetaType<QSharedPointer<ITableItem>>("QSharedPointer<ITableItem>");
  qRegisterMetaType<QList<QWeakPointer<ITableItem>>>("QList<QWeakPointer<ITableItem>>");
  qRegisterMetaType<QWeakPointer<ITableItem>>("QWeakPointer<ITableItem>");
  qRegisterMetaType<QPoint>("QPoint");
  setEditTriggers(QAbstractItemView::NoEditTriggers);


  initModels();
  initHeaders(sampleData.data());
  initHeaderSignal(sampleData.data());
  initInternalSignals();
}

ACustomTable::~ACustomTable()
{
  this->setParent(nullptr);

  if (!_proxyModel.isNull())
  {
    _proxyModel->setSourceModel(nullptr);
  }

  this->setModel(nullptr);

  if (!_manager.isNull())
  {
    _manager->setParent(nullptr);
  }
}

void  ACustomTable::initInternalSignals()
{
  connect(this->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &ACustomTable::onSelectionChanged, Qt::QueuedConnection);
  connect(this, &ACustomTable::customContextMenuRequested, this,
          &ACustomTable::onCustomContextMenuRequested, Qt::QueuedConnection);
  connect(this, &ACustomTable::clicked, this,
          &ACustomTable::onClicked, Qt::QueuedConnection);
  connect(this, &ACustomTable::doubleClicked, this,
          &ACustomTable::onDoubleClicked, Qt::QueuedConnection);

  connect(_manager.data(), &ATableManager::itemChanged, [ = ](QStandardItem *item)
    {
      if (nullptr != item)
      {
        if (item->isCheckable())
        {
          auto id = static_cast<IdType>(item->data(TABLE_MANAGER_ID_ROLE).toLongLong());
          auto it = _manager->getItem(id);

          if (!it.isNull())
          {
            emit itemCheckStateChanged(it, item->checkState() == Qt::Checked, item->column());
          }
        }
      }
    });
}

quint32  ACustomTable::getTableId() const
{
  return _tableId;
}

void  ACustomTable::updateHeaderItems()
{
  initHeaders(_sampleData.data());
}

QStringList  ACustomTable::getHeaders() const
{
  return _sampleData->getHeaders(_tableId);
}

void  ACustomTable::filterRegChanged(const QString &reg)
{
  if (!_proxyModel.isNull())
  {
    _proxyModel->setFilterRegExp(QRegExp(reg.trimmed().replace(" ", "|"), Qt::CaseInsensitive, QRegExp::PatternSyntax(0)));
    _proxyModel->invalidate();
  }
}

void  ACustomTable::addItem(QSharedPointer<ITableItem> a)
{
  Q_ASSERT(!a.isNull());
  emit  privateInternalSignal(a, false);
}

void  ACustomTable::updateItem(QSharedPointer<ITableItem> a)
{
  Q_ASSERT(!a.isNull());
  emit  privateInternalSignal(a, false);
}

void  ACustomTable::deleteItem(QSharedPointer<ITableItem> a)
{
  Q_ASSERT(!a.isNull());
  emit  privateInternalSignal(a, true);
}

void  ACustomTable::onClicked(const QModelIndex &index)
{
  auto  sIdx = _proxyModel->mapToSource(index);

  if (sIdx.isValid())
  {
    IdType  itm = static_cast<IdType>(_manager->data(sIdx,
                                                     ATableManager::getIdRole()).toLongLong());
    emit  itemClicked(_manager->getItem(itm));
  }
}

void  ACustomTable::onDoubleClicked(const QModelIndex &index)
{
  auto  sIdx = _proxyModel->mapToSource(index);

  if (sIdx.isValid())
  {
    auto  itm = static_cast<IdType>(_manager->data(sIdx,
                                                   ATableManager::getIdRole()).toLongLong());
    emit  itemDoubleClicked(_manager->getItem(itm));
  }
}

void  ACustomTable::onCustomContextMenuRequested(const QPoint &pos)
{
  auto  index = indexAt(pos);
  auto  sIdx  = _proxyModel->mapToSource(index);

  if (sIdx.isValid())
  {
    auto  itm = static_cast<IdType>(_manager->data(sIdx,
                                                   ATableManager::getIdRole()).toLongLong());
    emit  itemContextMenuRequested(_manager->getItem(itm), mapToGlobal(pos));
  }
}

QList<QWeakPointer<ITableItem>>  ACustomTable::getSelectedItems(const QItemSelection &selected)
{
  QMap<IdType, QWeakPointer<ITableItem>>  ret;

  foreach(auto var, selected)
  {
    foreach(auto var1, var.indexes())
    {
      auto  sIdx = _proxyModel->mapToSource(var1);

      if (sIdx.isValid())
      {
        auto  itm = static_cast<IdType>(_manager->data(sIdx,
                                                       ATableManager::getIdRole()).toLongLong());
        ret.insert(itm, _manager->getItem(itm));
      }
    }
  }

  return ret.values();
}

int  ACustomTable::getAutoUpdateInterval() const
{
  return _manager.isNull() ? -1 : _manager->getAutoUpdateInterval();
}

void  ACustomTable::setAutoUpdateInterval(int a)
{
  if (!_manager.isNull()) { _manager->setAutoUpdateInterval(a); }
}

void  ACustomTable::onSelectionChanged(const QItemSelection &selected, const QItemSelection&)
{
  emit  itemsSelectionChanged(getSelectedItems(selected));
}

void  ACustomTable::initModels()
{
  _manager = QSharedPointer<ATableManager>(new ATableManager(_tableId, nullptr));

  _proxyModel = QSharedPointer<ASortFilterProxyModel>(new ASortFilterProxyModel());
  _proxyModel->setSourceModel(_manager.data());
  this->setModel(_proxyModel.data());
  _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  this->setSortingEnabled(true);
  connect(this, &ACustomTable::privateInternalSignal,
          _manager.data(), &ATableManager::changeItem, Qt::QueuedConnection);
  connect(this, &ACustomTable::privateClearSignale,
          _manager.data(), &ATableManager::clearAll, Qt::QueuedConnection);
}

void  ACustomTable::initHeaders(ITableItem *sampleData)
{
  if (!_manager.isNull())
  {
    _manager->setHorizontalHeaderLabels(sampleData->getHeaders(_tableId));
    _proxyModel->invalidate();

    for (int var = 0; var < _manager->columnCount(); ++var)
    {
      resizeColumnToContents(var);
    }
  }
}

void  ACustomTable::initHeaderSignal(ITableItem *sampleData)
{
  if (header() != nullptr)
  {
    auto  ll = sampleData->getHeaders(_tableId);

    if (ll.count() > 1)
    {
      for (int var = 1; var < ll.count(); ++var)
      {
        QAction *ac = new QAction(ll.at(var));
        ac->setCheckable(true);
        ac->setChecked(true);
        ac->setData(var);
        _headerMenu.addAction(ac);
        connect(ac, &QAction::toggled, [ = ](bool a)
          {
            if (a)
            {
              this->showColumn(var);
            }
            else
            {
              this->hideColumn(var);
            }
          });
      }
    }

    header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header(), &QHeaderView::customContextMenuRequested,
            [ = ](const QPoint &pos)
      {
        _headerMenu.popup(this->mapToGlobal(pos));
      });
  }
}

QList<int>  ACustomTable::getFilteredColumns() const
{
  return _filteredColumns;
}

void  ACustomTable::setFilteredColumns(const QList<int> &filteredColumns, bool matchAllColumns)
{
  _filteredColumns = filteredColumns;

  if (!_proxyModel.isNull())
  {
    _proxyModel->setFilteredColumns(filteredColumns, matchAllColumns);
  }
}

void  ACustomTable::activeItemsContectMenu(bool a)
{
  this->setContextMenuPolicy(a ? Qt::CustomContextMenu : Qt::NoContextMenu);
}

QWeakPointer<ITableItem>  ACustomTable::getItem(IdType a) const
{
  return _manager->getItem(a);
}

int  ACustomTable::getRowCount() const
{
  return _manager->getRowCount();
}

int  ACustomTable::getAllItemCount() const
{
  return _manager->getAllItemCount();
}

void  ACustomTable::clearTable()
{
  emit  privateClearSignale();
}

void  ACustomTable::selectItem(IdType s)
{
  auto  d = _manager->getItemAllColumns(s);

  if (!d.isEmpty())
  {
    auto  idx = _proxyModel->mapFromSource(d.at(0)->index());
    selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect
                             | QItemSelectionModel::Rows
                             | QItemSelectionModel::Current);
    scrollTo(_proxyModel->mapFromSource(d.at(0)->index()));
  }
}

QWeakPointer<ATableManager>  ACustomTable::getManager() const
{
  return _manager;
}

void  ACustomTable::setManager(const QSharedPointer<ATableManager> &manager)
{
  _manager = manager;
}

QList<int>  ASortFilterProxyModel::filteredColumns() const
{
  return _filteredColumns;
}

void  ASortFilterProxyModel::setFilteredColumns(const QList<int> &filteredColumns, bool matchAllColumns)
{
  _corespond       = matchAllColumns;
  _filteredColumns = filteredColumns;
}

bool  ASortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
  int  col = _filteredColumns.isEmpty()
             ? sourceModel()->columnCount() : _filteredColumns.count();
  int  matchCountDown = _corespond ? col : 1;

  for (int var = 0; var < sourceModel()->columnCount(); ++var)
  {
    if (_filteredColumns.isEmpty() || _filteredColumns.contains(var))
    {
      auto  idx = sourceModel()->index(source_row, var, source_parent);

      if (idx.isValid())
      {
        QString  text = sourceModel()->data(idx, filterRole()).toString();

        if (text.contains(filterRegExp()))
        {
          matchCountDown--;
        }
      }
    }
  }

  return 0 >= matchCountDown;
}

// void ACustomTable::selectItems(QList<IdType> s)
// {
// bool change=true;

// auto old=getSelectedItems(selectionModel()->selection());
// if(old.size()==s.size()){
// if(!old.isEmpty()){
// if(!old.first().isNull()&& !old.last().isNull())
// {
// if(old.first().lock()->getId()==s.first()&&
// old.last().lock()->getId()==s.last())
// {
// change=false;
// }
// }
// }
// }

// if(!change)return;

// QModelIndex ff;
// selectionModel()->select(QModelIndex(),QItemSelectionModel::Clear);
// foreach (auto var, s) {
// auto d=_manager->getItemAllColumns(var);
// if(!d.isEmpty()){
// auto idx=_proxyModel->mapFromSource(d.at(0)->index());

// selectionModel()->select(idx,QItemSelectionModel::Select|
// QItemSelectionModel::Rows);
// }

// if(!d.isEmpty() && ff.isValid())
// {
// ff=_proxyModel->mapFromSource(d.at(0)->index());
// }
// }
// scrollTo(_proxyModel->mapFromSource(ff));
// }
}
