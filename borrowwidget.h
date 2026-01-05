#ifndef BORROWWIDGET_H
#define BORROWWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QSqlQuery>
#include "borrowmodel.h"
#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class BorrowWidget; }
QT_END_NAMESPACE

class BorrowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BorrowWidget(QWidget *parent = nullptr);
    ~BorrowWidget();

private slots:
    void onBorrowBook();
    void onReturnBook();
    void onSearch();
    void onClearSearch();
    void onSelectionChanged();
    void updateBorrowDays();

private:
    Ui::BorrowWidget *ui;
    BorrowModel *model;
    
    void setupUI();
    void setupModel();
    void updateReturnDate();
    int getSelectedRow();
};

#endif // BORROWWIDGET_H
