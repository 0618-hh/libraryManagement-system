#ifndef READERWIDGET_H
#define READERWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include "readermodel.h"
#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ReaderWidget; }
QT_END_NAMESPACE

class ReaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReaderWidget(QWidget *parent = nullptr);
    ~ReaderWidget();

private slots:
    void onAddReader();
    void onEditReader();
    void onDeleteReader();
    void onSearch();
    void onClearSearch();
    void onSelectionChanged();

private:
    Ui::ReaderWidget *ui;
    ReaderModel *model;
    
    void setupUI();
    void setupModel();
    void clearEditForm();
    void loadReaderToForm(int row);
    int getSelectedRow();
};

#endif // READERWIDGET_H
