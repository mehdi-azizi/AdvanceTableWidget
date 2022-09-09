#ifndef ATABLEFILTERWIDGET_H
#define ATABLEFILTERWIDGET_H


#include <QWidget>
class QLineEdit;
class QToolButton;
class QHBoxLayout;
class QMenu;
namespace CustomTable
{
class ACustomTable;
class ATableFilterWidget: public QWidget
{
  Q_OBJECT

public:
  explicit ATableFilterWidget(ACustomTable *table, QWidget *parent = nullptr);

signals:
private slots:
  void  selectColumns(bool);

private:
  void  initView();

  void  fillMenu();

private:
  ACustomTable     *_table;
  QLineEdit        *le_filter;
  QToolButton      *tb_column;
  QHBoxLayout      *_layout;
  QMenu            *_menu;
  QList<QAction *>  _actions;
  QAction          *_actionAll;
  QAction          *_matchAllColumns;
  bool              _allState;
  bool              _changingAct;
  bool              _noSearch;
};
}

#endif // ATABLEFILTERWIDGET_H
