#include "bookmodel.h"
#include <QColor>
#include <QBrush>
#include <QStringList>

BookModel::BookModel(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
{
    setTable("books");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    // 设置表头
    setHeaderData(0, Qt::Horizontal, "ID");
    setHeaderData(1, Qt::Horizontal, "ISBN");
    setHeaderData(2, Qt::Horizontal, "书名");
    setHeaderData(3, Qt::Horizontal, "作者");
    setHeaderData(4, Qt::Horizontal, "出版社");
    setHeaderData(5, Qt::Horizontal, "出版日期");
    setHeaderData(6, Qt::Horizontal, "总册数");
    setHeaderData(7, Qt::Horizontal, "可借册数");
    
    select();
}

QVariant BookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0 || index.column() == 6 || index.column() == 7) {
            return Qt::AlignCenter;
        }
    }
    
    if (role == Qt::ForegroundRole) {
        int availableCol = 7;
        if (index.column() == availableCol) {
            int available = QSqlTableModel::data(this->index(index.row(), availableCol), Qt::DisplayRole).toInt();
            if (available == 0) {
                return QBrush(QColor(255, 0, 0)); // 红色表示无库存
            }
        }
    }
    
    return QSqlTableModel::data(index, role);
}

Qt::ItemFlags BookModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    // ID列不可编辑
    if (index.column() == 0) {
        flags &= ~Qt::ItemIsEditable;
    }
    return flags;
}

void BookModel::setFilter(const QString &isbn, const QString &title, const QString &author)
{
    QString filter;
    QStringList conditions;
    
    if (!isbn.isEmpty()) {
        conditions << QString("isbn LIKE '%%1%'").arg(isbn);
    }
    if (!title.isEmpty()) {
        conditions << QString("title LIKE '%%1%'").arg(title);
    }
    if (!author.isEmpty()) {
        conditions << QString("author LIKE '%%1%'").arg(author);
    }
    
    if (!conditions.isEmpty()) {
        filter = conditions.join(" AND ");
    }
    
    QSqlTableModel::setFilter(filter);
    select();
}

void BookModel::clearFilter()
{
    setFilter("", "", "");
    select();
}
