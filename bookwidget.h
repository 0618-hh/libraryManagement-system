#ifndef BOOKWIDGET_H
#define BOOKWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include "bookmodel.h"
#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class BookWidget; }
QT_END_NAMESPACE

class BookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BookWidget(QWidget *parent = nullptr);
    ~BookWidget();

private slots:
    void onAddBook();
    void onEditBook();
    void onDeleteBook();
    void onSearch();
    void onClearSearch();
    void onSelectionChanged();

private:
    Ui::BookWidget *ui;
    BookModel *model;
    
    void setupUI();
    void setupModel();
    void clearEditForm();
    void loadBookToForm(int row);
    int getSelectedRow();
};

#endif // BOOKWIDGET_H
