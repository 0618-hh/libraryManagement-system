#ifndef BOOKMODEL_H
#define BOOKMODEL_H

#include <QSqlTableModel>
#include <QObject>
#include <QVariant>
#include <QModelIndex>
#include <QString>

class BookModel : public QSqlTableModel
{
    Q_OBJECT

public:
    explicit BookModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    void setFilter(const QString &isbn, const QString &title, const QString &author);
    void clearFilter();
};

#endif // BOOKMODEL_H
