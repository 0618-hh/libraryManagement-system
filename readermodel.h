#ifndef READERMODEL_H
#define READERMODEL_H

#include <QSqlTableModel>
#include <QObject>
#include <QVariant>
#include <QModelIndex>
#include <QString>

class ReaderModel : public QSqlTableModel
{
    Q_OBJECT

public:
    explicit ReaderModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    void setFilter(const QString &readerId, const QString &name);
    void clearFilter();
};

#endif // READERMODEL_H
