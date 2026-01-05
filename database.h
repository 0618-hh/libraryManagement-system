#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QDate>
#include <QList>
#include <QPair>
#include <QString>

class Database : public QObject
{
    Q_OBJECT

public:
    static Database& instance();
    bool initializeDatabase();
    QSqlDatabase getDatabase() const;
    
    // 图书相关操作
    bool addBook(const QString &isbn, const QString &title, const QString &author, 
                 const QString &publisher, const QDate &publishDate, int totalCopies);
    bool updateBook(int bookId, const QString &isbn, const QString &title, 
                    const QString &author, const QString &publisher, 
                    const QDate &publishDate, int totalCopies);
    bool deleteBook(int bookId);
    bool hasBookBorrows(int bookId); // 检查图书是否有借阅记录
    bool isBookIsbnExists(const QString &isbn, int excludeBookId = -1); // 检查ISBN是否已存在
    
    // 读者相关操作
    bool addReader(const QString &readerId, const QString &name, 
                   const QString &phone, const QString &email);
    bool updateReader(int readerId, const QString &readerIdStr, const QString &name,
                      const QString &phone, const QString &email);
    bool deleteReader(int readerId);
    bool hasReaderBorrows(int readerId); // 检查读者是否有借阅记录
    bool isReaderIdExists(const QString &readerId, int excludeReaderId = -1); // 检查读者编号是否已存在
    
    // 借阅相关操作
    bool borrowBook(int bookId, int readerId, const QDate &borrowDate, const QDate &returnDate);
    bool returnBook(int borrowId);
    QList<QPair<int, QDate>> getOverdueBorrows(); // 返回逾期借阅ID和应还日期
    // 获取借阅记录详细信息（用于还书确认）
    struct BorrowInfo {
        int borrowId;
        QString bookTitle;
        QString readerName;
        QDate borrowDate;
        QDate returnDate;
        bool isOverdue;
        int overdueDays;
    };
    BorrowInfo getBorrowInfo(int borrowId); // 获取借阅记录详细信息
    
private:
    Database(QObject *parent = nullptr);
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    QSqlDatabase db;
    QString dbPath;
    
    bool createTables();
};

#endif // DATABASE_H
