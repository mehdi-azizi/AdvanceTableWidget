#include "ATableFilterWidget.h"
#include "ACustomTable.h"
#include <qlineedit.h>
#include <QToolButton>
#include <QHBoxLayout>
#include <QMenu>

namespace CustomTable
{
ATableFilterWidget::ATableFilterWidget(ACustomTable *table, QWidget *parent):
  QWidget(parent), _table(table)
{
  Q_ASSERT(nullptr != table);
  initView();
  fillMenu();
  selectColumns(true);
}

void  ATableFilterWidget::selectColumns(bool)
{
  if (_changingAct) { return; }

  QList<int>  ret;

  for (int var = 0; var < _actions.size(); ++var)
  {
    if (_actions.at(var)->isChecked())
    {
      ret.append(var);
    }
    else { _allState = false; }
  }

  _noSearch = ret.isEmpty();
  _table->setFilteredColumns(ret, _matchAllColumns->isChecked());
  tb_column->setText(QString("%1 (%2)").arg(QObject::tr("in Col.")).
                     arg(ret.size()));
  _table->filterRegChanged(_noSearch ? "" : le_filter->text());
}

void  ATableFilterWidget::initView()
{
  _layout   = new QHBoxLayout(this);
  le_filter = new QLineEdit(this);
  tb_column = new QToolButton(this);
  tb_column->setToolTip(QObject::tr("Select which columns search for the match. "));
  _layout->addWidget(le_filter);
  _layout->addWidget(tb_column);
  _layout->setStretch(0, 1);

  le_filter->setPlaceholderText(QObject::tr("Search..."));
  le_filter->setToolTip(QObject::tr("use Space to add more conditions.(example: \"a-1 a-2 a-4\" )"));
  connect(le_filter, &QLineEdit::textChanged,
          [ = ](const QString &reg)
    {
      _table->filterRegChanged(_noSearch ? "" : reg);
    });
  connect(tb_column, &QToolButton::clicked, [ = ]
    {
      _menu->popup(mapToGlobal(tb_column->geometry().topRight()));
    });
}

void  ATableFilterWidget::fillMenu()
{
  auto  h = _table->getHeaders();

  _menu        = new QMenu(this);
  _allState    = true;
  _changingAct = false;
  _noSearch    = false;

  _matchAllColumns = new QAction(QObject::tr("Mutch all selected col.s"));
  _matchAllColumns->setCheckable(true);
  _matchAllColumns->setChecked(false);
  _matchAllColumns->setToolTip(QObject::tr("The condition has to match all selected columns."));
  _menu->addAction(_matchAllColumns);
  connect(_matchAllColumns, &QAction::toggled, [ = ](bool)
    {
      selectColumns(true);
    });

  _menu->addSeparator();

  _actionAll = new QAction(QObject::tr("select/deselect All"));
  _menu->addAction(_actionAll);
  connect(_actionAll, &QAction::triggered, [ = ](bool)
    {
      _allState    = !_allState;
      _changingAct = true;

      for (int var = 0; var < _actions.size(); ++var)
      {
        _actions.at(var)->setChecked(_allState);
      }

      _changingAct = false;
      selectColumns(true);
    });

  for (int var = 0; var < h.size(); ++var)
  {
    QAction *ac = new QAction(h.at(var));
    ac->setCheckable(true);
    ac->setChecked(true);
    _menu->addAction(ac);
    _actions.append(ac);
    connect(ac, &QAction::toggled, this, &ATableFilterWidget::selectColumns);
  }
}
}
