#include "borrowmodel.h"
#include <QColor>
#include <QBrush>
#include <QDate>
#include <QSqlQuery>
#include <QSqlRelation>
#include <QStringList>

BorrowModel::BorrowModel(QObject *parent, QSqlDatabase db)
    : QSqlRelationalTableModel(parent, db)
{
    setTable("borrows");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    // 设置关联
    setRelation(1, QSqlRelation("books", "id", "title"));
    setRelation(2, QSqlRelation("readers", "id", "name"));
    
    // 设置表头
    setHeaderData(0, Qt::Horizontal, "ID");
    setHeaderData(1, Qt::Horizontal, "图书");
    setHeaderData(2, Qt::Horizontal, "读者");
    setHeaderData(3, Qt::Horizontal, "借阅日期");
    setHeaderData(4, Qt::Horizontal, "应还日期");
    setHeaderData(5, Qt::Horizontal, "实际归还日期");
    setHeaderData(6, Qt::Horizontal, "状态");
    
    select();
}

QVariant BorrowModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0 || index.column() == 3 || index.column() == 4 || index.column() == 5) {
            return Qt::AlignCenter;
        }
    }
    
    if (role == Qt::ForegroundRole) {
        int statusCol = 6;
        int returnDateCol = 4;
        
        if (index.column() == statusCol || index.column() == returnDateCol) {
            QString status = QSqlRelationalTableModel::data(this->index(index.row(), statusCol), Qt::DisplayRole).toString();
            if (status == "borrowed") {
                QDate returnDate = QDate::fromString(
                    QSqlRelationalTableModel::data(this->index(index.row(), returnDateCol), Qt::DisplayRole).toString(),
                    "yyyy-MM-dd");
                if (returnDate < QDate::currentDate()) {
                    return QBrush(QColor(255, 0, 0)); // 红色表示逾期
                }
            }
        }
    }
    
    return QSqlRelationalTableModel::data(index, role);
}

Qt::ItemFlags BorrowModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlRelationalTableModel::flags(index);
    // 所有列都设为只读，通过专门的界面进行编辑
    flags &= ~Qt::ItemIsEditable;
    return flags;
}

void BorrowModel::setFilter(const QString &bookTitle, const QString &readerName, const QString &status)
{
    QString filter;
    QStringList conditions;
    
    if (!bookTitle.isEmpty()) {
        // 需要通过JOIN来过滤
        conditions << QString("book_id IN (SELECT id FROM books WHERE title LIKE '%%1%')").arg(bookTitle);
    }
    if (!readerName.isEmpty()) {
        conditions << QString("reader_id IN (SELECT id FROM readers WHERE name LIKE '%%1%')").arg(readerName);
    }
    if (!status.isEmpty()) {
        conditions << QString("status = '%1'").arg(status);
    }
    
    if (!conditions.isEmpty()) {
        filter = conditions.join(" AND ");
    }
    
    QSqlRelationalTableModel::setFilter(filter);
    select();
}

void BorrowModel::clearFilter()
{
    setFilter("", "", "");
    select();
}
