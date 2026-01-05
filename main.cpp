#include "mainwindow.h"
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
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
