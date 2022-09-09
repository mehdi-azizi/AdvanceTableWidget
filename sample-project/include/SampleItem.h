#ifndef SAMPLEITEM_H
#define SAMPLEITEM_H
#include "custom-table/ITableItem.h"
namespace SamplePrj
{
using namespace CustomTable;

class SampleItem: public ITableItem
{
  static QStringList  Headers;

public:
  explicit SampleItem(IdType invalidValue = INVALID_ID_VALUE);

  explicit SampleItem(IdType  id,
                      QString name,
                      bool    state,
                      QIcon   icon,
                      IdType  invalidValue = INVALID_ID_VALUE);

public:
  // ITableItem interface
  const QStringList &getHeaders(quint32) const override;

  void  updateFromSource(quint32 tableId)override;

  bool  getCheckBoxEditability(int column)override;

  bool  isColumnIsCheckBox(int column)override;

  IdType  getId() const;

  void  setId(const IdType &id);

  QString  getName() const;

  void  setName(const QString &name);

  bool  getState() const;

  void  setState(bool state);

  QIcon  getIcon() const;

  void  setIcon(const QIcon &icon);

private:
  QString  _name;
  bool     _state;
  QIcon    _icon;
};

typedef   QSharedPointer<SampleItem> ItemPtr;
}
#endif // SAMPLEITEM_H
