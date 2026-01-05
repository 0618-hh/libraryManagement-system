#ifndef BORROWMODEL_H
#define BORROWMODEL_H

#include <QSqlRelationalTableModel>
#include <QObject>
#include <QVariant>
#include <QModelIndex>
#include <QString>
#include <QSqlRelation>

class BorrowModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit BorrowModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    void setFilter(const QString &bookTitle, const QString &readerName, const QString &status);
    void clearFilter();
};

#endif // BORROWMODEL_H
