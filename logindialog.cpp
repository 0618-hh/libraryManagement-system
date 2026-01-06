#include "logindialog.h"
#include "ui_logindialog.h"
#include <QFont>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setupUI();
    
    // 设置窗口属性
    setWindowTitle("登录 - 图书借阅管理系统");
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setModal(true);
    
    // 设置密码输入框的回车键响应
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLogin);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // 标题
    QLabel *titleLabel = new QLabel("图书借阅管理系统", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    
    // 副标题
    QLabel *subtitleLabel = new QLabel("请登录", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: gray;");
    mainLayout->addWidget(subtitleLabel);
    
    mainLayout->addSpacing(10);
    
    // 表单区域
    QFormLayout *formLayout = new QFormLayout;
    formLayout->setSpacing(10);
    formLayout->setLabelAlignment(Qt::AlignRight);
    
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("请输入用户名");
    usernameEdit->setMinimumWidth(250);
    formLayout->addRow("用户名:", usernameEdit);
    
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumWidth(250);
    formLayout->addRow("密码:", passwordEdit);
    
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(10);
    
    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(10);
    
    loginButton = new QPushButton("登录", this);
    loginButton->setDefault(true);
    loginButton->setMinimumWidth(100);
    loginButton->setStyleSheet("QPushButton { background-color: #0078d4; color: white; padding: 8px; border-radius: 4px; }"
                               "QPushButton:hover { background-color: #005a9e; }"
                               "QPushButton:pressed { background-color: #004578; }");
    
    cancelButton = new QPushButton("取消", this);
    cancelButton->setMinimumWidth(100);
    cancelButton->setStyleSheet("QPushButton { padding: 8px; border-radius: 4px; }");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    
    // 连接信号
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::onCancel);
    
    // 设置焦点到用户名输入框
    usernameEdit->setFocus();
}

void LoginDialog::onLogin()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    
    // 验证输入
    if (username.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入用户名！");
        usernameEdit->setFocus();
        return;
    }
    
    if (password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入密码！");
        passwordEdit->setFocus();
        return;
    }
    
    // 验证登录信息
    if (validateLogin(username, password)) {
        accept(); // 关闭对话框并返回 QDialog::Accepted
    } else {
        QMessageBox::critical(this, "登录失败", "用户名或密码错误！\n\n请重试。");
        passwordEdit->clear();
        passwordEdit->setFocus();
    }
}

void LoginDialog::onCancel()
{
    reject(); // 关闭对话框并返回 QDialog::Rejected
}

bool LoginDialog::validateLogin(const QString &username, const QString &password)
{
    // 验证用户名和密码
    return (username == "黄奕辉" && password == "123456");
}
