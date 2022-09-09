#ifndef ITEMGENERATOR_H
#define ITEMGENERATOR_H

#include "SampleItem.h"
#include <QThread>
namespace SamplePrj
{
using namespace CustomTable;


class ItemGenerator: public QThread
{
  Q_OBJECT
  enum RunState
  {
    RS_STOP,
    RS_PAUSE,
    RS_START
  };

public:
  explicit ItemGenerator(int suffixId, int itemCount, unsigned long stepInterval, QObject *parent = nullptr);

  ~ItemGenerator();

  void  generate();

  void  pause();

  void  resume();

  void  stop();

signals:
  void  changeItem(QSharedPointer<ITableItem>, bool isDeleted);

private:
  QMap<IdType, ItemPtr>  _items;
  QList<IdType>          _deleted;
  RunState               _state;
  int                    _suffixId;
  int                    _itemCount;
  unsigned long          _stepInterval;
  QList<QIcon>           _icons;

  void  addItem(IdType _parent = 0);

  void  updateItem();

  void  deleteItem();

  // QThread interface

protected:
  void  run();
};

typedef   QSharedPointer<ItemGenerator> GeneratorPtr;
}
#endif // ITEMGENERATOR_H
