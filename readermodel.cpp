#include "readermodel.h"
#include <QStringList>

ReaderModel::ReaderModel(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
{
    setTable("readers");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    // 设置表头
    setHeaderData(0, Qt::Horizontal, "ID");
    setHeaderData(1, Qt::Horizontal, "读者编号");
    setHeaderData(2, Qt::Horizontal, "姓名");
    setHeaderData(3, Qt::Horizontal, "电话");
    setHeaderData(4, Qt::Horizontal, "邮箱");
    
    select();
}

QVariant ReaderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0) {
            return Qt::AlignCenter;
        }
    }
    
    return QSqlTableModel::data(index, role);
}

Qt::ItemFlags ReaderModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    // ID列不可编辑
    if (index.column() == 0) {
        flags &= ~Qt::ItemIsEditable;
    }
    return flags;
}

void ReaderModel::setFilter(const QString &readerId, const QString &name)
{
    QString filter;
    QStringList conditions;
    
    if (!readerId.isEmpty()) {
        conditions << QString("reader_id LIKE '%%1%'").arg(readerId);
    }
    if (!name.isEmpty()) {
        conditions << QString("name LIKE '%%1%'").arg(name);
    }
    
    if (!conditions.isEmpty()) {
        filter = conditions.join(" AND ");
    }
    
    QSqlTableModel::setFilter(filter);
    select();
}

void ReaderModel::clearFilter()
{
    setFilter("", "");
    select();
}
