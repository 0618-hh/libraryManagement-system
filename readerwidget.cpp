#include "readerwidget.h"
#include "ui_readerwidget.h"
#include <QHeaderView>

ReaderWidget::ReaderWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReaderWidget)
{
    ui->setupUi(this);
    setupUI();
    setupModel();
}

ReaderWidget::~ReaderWidget()
{
    delete ui;
}

void ReaderWidget::setupUI()
{
    // 设置按钮提示信息
    ui->editButton->setToolTip("请先在表格中选择一条记录");
    ui->deleteButton->setToolTip("请先在表格中选择一条记录");
    
    // 连接信号
    connect(ui->searchButton, &QPushButton::clicked, this, &ReaderWidget::onSearch);
    connect(ui->clearButton, &QPushButton::clicked, this, &ReaderWidget::onClearSearch);
    connect(ui->addButton, &QPushButton::clicked, this, &ReaderWidget::onAddReader);
    connect(ui->editButton, &QPushButton::clicked, this, &ReaderWidget::onEditReader);
    connect(ui->deleteButton, &QPushButton::clicked, this, &ReaderWidget::onDeleteReader);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ReaderWidget::onSelectionChanged);
    
    // 确保表格可以接收点击事件
    connect(ui->tableView, &QTableView::clicked, this, [this](const QModelIndex &index) {
        Q_UNUSED(index);
        // 点击表格时确保选择正确
        onSelectionChanged();
    });
}

void ReaderWidget::setupModel()
{
    model = new ReaderModel(this, Database::instance().getDatabase());
    ui->tableView->setModel(model);
    
    // 设置列宽
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setColumnWidth(0, 50);
    ui->tableView->setColumnWidth(1, 120);
    ui->tableView->setColumnWidth(2, 120);
    ui->tableView->setColumnWidth(3, 150);
    ui->tableView->setColumnWidth(4, 200);
    
    // 隐藏不需要的列
    ui->tableView->hideColumn(5); // created_at
}

void ReaderWidget::onAddReader()
{
    if (ui->editReaderId->text().isEmpty() || ui->editName->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "请填写读者编号和姓名！");
        return;
    }
    
    if (Database::instance().addReader(ui->editReaderId->text(), ui->editName->text(),
                                       ui->editPhone->text(), ui->editEmail->text())) {
        QMessageBox::information(this, "成功", "读者添加成功！");
        clearEditForm();
        model->select();
    } else {
        QMessageBox::critical(this, "错误", "读者添加失败！可能是读者编号已存在。");
    }
}

void ReaderWidget::onEditReader()
{
    int row = getSelectedRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请选择要修改的读者！");
        return;
    }
    
    int readerId = model->data(model->index(row, 0)).toInt();
    
    if (ui->editReaderId->text().isEmpty() || ui->editName->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "请填写读者编号和姓名！");
        return;
    }
    
    if (Database::instance().updateReader(readerId, ui->editReaderId->text(), ui->editName->text(),
                                          ui->editPhone->text(), ui->editEmail->text())) {
        QMessageBox::information(this, "成功", "读者修改成功！");
        clearEditForm();
        model->select();
        
        // 尝试重新选中修改的记录
        for (int i = 0; i < model->rowCount(); ++i) {
            if (model->data(model->index(i, 0)).toInt() == readerId) {
                ui->tableView->selectRow(i);
                break;
            }
        }
        
        ui->editButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
    } else {
        QString errorMsg = "读者修改失败！";
        if (Database::instance().isReaderIdExists(ui->editReaderId->text(), readerId)) {
            errorMsg += "\n原因：读者编号已存在，请使用其他编号。";
        }
        QMessageBox::critical(this, "错误", errorMsg);
    }
}

void ReaderWidget::onDeleteReader()
{
    int row = getSelectedRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请选择要删除的读者！");
        return;
    }
    
    int readerId = model->data(model->index(row, 0)).toInt();
    QString name = model->data(model->index(row, 2)).toString();
    
    // 检查是否有借阅记录
    if (Database::instance().hasReaderBorrows(readerId)) {
        QMessageBox::warning(this, "无法删除", 
                            QString("无法删除读者 %1！\n\n原因：该读者存在借阅记录，请先处理所有借阅记录后再删除。").arg(name));
        return;
    }
    
    int ret = QMessageBox::question(this, "确认删除", 
                                    QString("确定要删除读者 %1 吗？\n\n此操作不可恢复！").arg(name),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (Database::instance().deleteReader(readerId)) {
            QMessageBox::information(this, "成功", "读者删除成功！");
            clearEditForm();
            model->select();
            ui->editButton->setEnabled(false);
            ui->deleteButton->setEnabled(false);
        } else {
            QMessageBox::critical(this, "错误", "读者删除失败！");
        }
    }
}

void ReaderWidget::onSearch()
{
    model->setFilter(ui->readerIdEdit->text(), ui->nameEdit->text());
}

void ReaderWidget::onClearSearch()
{
    ui->readerIdEdit->clear();
    ui->nameEdit->clear();
    model->clearFilter();
}

void ReaderWidget::onSelectionChanged()
{
    int row = getSelectedRow();
    if (row >= 0) {
        loadReaderToForm(row);
        ui->editButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
        ui->editButton->setToolTip("修改当前选中的读者信息");
        ui->deleteButton->setToolTip("删除当前选中的读者");
    } else {
        clearEditForm();
        ui->editButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        ui->editButton->setToolTip("请先在表格中选择一条记录");
        ui->deleteButton->setToolTip("请先在表格中选择一条记录");
    }
}

void ReaderWidget::clearEditForm()
{
    ui->editReaderId->clear();
    ui->editName->clear();
    ui->editPhone->clear();
    ui->editEmail->clear();
}

void ReaderWidget::loadReaderToForm(int row)
{
    ui->editReaderId->setText(model->data(model->index(row, 1)).toString());
    ui->editName->setText(model->data(model->index(row, 2)).toString());
    ui->editPhone->setText(model->data(model->index(row, 3)).toString());
    ui->editEmail->setText(model->data(model->index(row, 4)).toString());
}

int ReaderWidget::getSelectedRow()
{
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) {
        return -1;
    }
    return indexes.first().row();
}
