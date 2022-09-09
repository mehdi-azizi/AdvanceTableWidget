#include "SampleItem.h"
namespace SamplePrj
{
QStringList  SampleItem::Headers = QStringList();

SampleItem::SampleItem(IdType invalidValue):
  ITableItem(invalidValue)
{
}

SampleItem::SampleItem(IdType  id,
                       QString name,
                       bool    state,
                       QIcon   icon,
                       IdType  invalidValue):
  ITableItem(invalidValue),
  _name(name), _state(state), _icon(icon)
{
  ITableItem::setId(id);
}

const QStringList& SampleItem::getHeaders(quint32) const
{
  if (SampleItem::Headers.isEmpty())
  {
    SampleItem::Headers << "ID" << "Name" << "IsUpdated" << "Icon";
  }

  return SampleItem::Headers;
}

void  SampleItem::updateFromSource(quint32 tableId)
{
  QList<QVariant>  dt;

  dt << QString::number(getId())
     << _name
     << _state
     << _icon;
  setData(tableId, dt);
}

bool  SampleItem::getCheckBoxEditability(int column)
{
  return 2 == column;
}

bool  SampleItem::isColumnIsCheckBox(int column)
{
  return 2 == column;
}

IdType  SampleItem::getId() const
{
  return ITableItem::getId();
}

void  SampleItem::setId(const IdType &id)
{
  ITableItem::setId(id);
}

QString  SampleItem::getName() const
{
  return _name;
}

void  SampleItem::setName(const QString &name)
{
  _name = name;
}

bool  SampleItem::getState() const
{
  return _state;
}

void  SampleItem::setState(bool state)
{
  _state = state;
}

QIcon  SampleItem::getIcon() const
{
  return _icon;
}

void  SampleItem::setIcon(const QIcon &icon)
{
  _icon = icon;
}
}
