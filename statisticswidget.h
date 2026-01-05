#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSqlQuery>
#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class StatisticsWidget; }
QT_END_NAMESPACE

class StatisticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsWidget(QWidget *parent = nullptr);
    ~StatisticsWidget();

private:
    Ui::StatisticsWidget *ui;
    
    void setupUI();
    void refreshStatistics();
    void loadTopBooks();
    void loadTopReaders();
    void loadOverdueBorrows();
};

#endif // STATISTICSWIDGET_H
