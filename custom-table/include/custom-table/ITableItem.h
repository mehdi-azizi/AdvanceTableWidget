#ifndef ITableItem_H
#define ITableItem_H
#include <qobject.h>
#include <qsharedpointer.h>
#include <QStandardItemModel>
#include <qmutex.h>
#include <qdebug.h>

typedef  quint64 IdType;
#define INVALID_ID_VALUE 0

namespace CustomTable
{
class ITableItem
{
public:
        ITableItem(IdType _parent = INVALID_ID_VALUE);

        virtual ~ITableItem();

public:
        bool  hasParent() const;

        IdType  getParent() const;

        bool  hasChild(IdType id) const;

        QList<QSharedPointer<ITableItem>>  getChildren() const;

        void  setParent(const IdType&);

        void  setChildren(const QList<QSharedPointer<ITableItem>>&);

        void  addChild(QSharedPointer<ITableItem> );

        void  removeChild(const IdType&);

        IdType  getId() const;

        void  setId(const IdType&);

        bool  isChildrenChanged() const;

        void  setChildrenChanged(bool);

        void  setData(quint32 tableId, const QList<QVariant>&);

        QList<QVariant>  getAllData(quint32 tableId);

        QVariant  getColumnData(quint32 tableId, int);

        bool  getUpdateNeeded() const;

        void  setUpdateNeeded(bool updateNeeded);

        ///
        /// \brief getHeaders : return the headers of table
        /// (It's call by Manager just once)
        /// \param tableId : ID of table, to use the Item in different tables
        /// \return : list of headers that shown to user, (order based on data)
        virtual const QStringList & getHeaders(quint32 tableId            = 1) const= 0;

        ///
        /// \brief updateFromSource : updates all data from source
        /// In this method you can proccess all you need then use setData() method to update the data.
        /// (This method is  better to be call in other thread than GUI)
        /// \param tableId : ID of table (0 means all Tables), to use the Item in different tables
        ///
        virtual void  updateFromSource(quint32 tableId      = 0)= 0;

        virtual bool  getCheckBoxEditability(int)
        {
                return false;
        }

        virtual bool  isColumnIsCheckBox(int)
        {
                return false;
        }

protected:
        IdType  _id;
        QMutex *_mutex;

private:
        IdType                             _parent;
        QList<QSharedPointer<ITableItem>>  _children;
        bool                               _isChildrenChanged;
        QMap<quint32, QList<QVariant>>     _data;
        bool                               _updateNeeded;
};
}
#endif // ITableItem_H
