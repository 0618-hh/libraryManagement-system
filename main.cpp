#include "mainwindow.h"
#include "logindialog.h"
#include <QApplication>
#include <QStyleFactory>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 设置应用程序信息
    app.setApplicationName("图书借阅管理系统");
    app.setOrganizationName("Library");
    
    // 显示登录对话框
    LoginDialog loginDialog;
    if (loginDialog.exec() != QDialog::Accepted) {
        // 用户取消登录，退出程序
        return 0;
    }
    
    // 登录成功，显示主窗口
    MainWindow window;
    window.show();
    
    return app.exec();
}
