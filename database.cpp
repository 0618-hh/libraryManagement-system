#include "database.h"
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

Database& Database::instance()
{
    static Database instance;
    return instance;
}

Database::Database(QObject *parent)
    : QObject(parent)
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dataPath)) {
        dir.mkpath(dataPath);
    }
    dbPath = dataPath + "/library.db";
}

Database::~Database()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool Database::initializeDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    
    if (!db.open()) {
        qDebug() << "无法打开数据库:" << db.lastError().text();
        return false;
    }
    
    return createTables();
}

bool Database::createTables()
{
    QSqlQuery query(db);
    
    // 创建图书表
    QString createBooksTable = R"(
        CREATE TABLE IF NOT EXISTS books (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            isbn TEXT UNIQUE NOT NULL,
            title TEXT NOT NULL,
            author TEXT NOT NULL,
            publisher TEXT,
            publish_date DATE,
            total_copies INTEGER DEFAULT 1,
            available_copies INTEGER DEFAULT 1,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createBooksTable)) {
        qDebug() << "创建图书表失败:" << query.lastError().text();
        return false;
    }
    
    // 创建读者表
    QString createReadersTable = R"(
        CREATE TABLE IF NOT EXISTS readers (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            reader_id TEXT UNIQUE NOT NULL,
            name TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createReadersTable)) {
        qDebug() << "创建读者表失败:" << query.lastError().text();
        return false;
    }
    
    // 创建借阅记录表
    QString createBorrowsTable = R"(
        CREATE TABLE IF NOT EXISTS borrows (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            book_id INTEGER NOT NULL,
            reader_id INTEGER NOT NULL,
            borrow_date DATE NOT NULL,
            return_date DATE NOT NULL,
            actual_return_date DATE,
            status TEXT DEFAULT 'borrowed',
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (book_id) REFERENCES books(id),
            FOREIGN KEY (reader_id) REFERENCES readers(id)
        )
    )";
    
    if (!query.exec(createBorrowsTable)) {
        qDebug() << "创建借阅表失败:" << query.lastError().text();
        return false;
    }
    
    // 创建索引
    query.exec("CREATE INDEX IF NOT EXISTS idx_books_isbn ON books(isbn)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_readers_reader_id ON readers(reader_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_borrows_status ON borrows(status)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_borrows_return_date ON borrows(return_date)");
    
    return true;
}

QSqlDatabase Database::getDatabase() const
{
    return db;
}

bool Database::addBook(const QString &isbn, const QString &title, const QString &author,
                       const QString &publisher, const QDate &publishDate, int totalCopies)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO books (isbn, title, author, publisher, publish_date, total_copies, available_copies) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(isbn);
    query.addBindValue(title);
    query.addBindValue(author);
    query.addBindValue(publisher);
    query.addBindValue(publishDate.toString("yyyy-MM-dd"));
    query.addBindValue(totalCopies);
    query.addBindValue(totalCopies);
    
    if (!query.exec()) {
        qDebug() << "添加图书失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::updateBook(int bookId, const QString &isbn, const QString &title,
                          const QString &author, const QString &publisher,
                          const QDate &publishDate, int totalCopies)
{
    // 检查ISBN是否与其他图书冲突
    if (isBookIsbnExists(isbn, bookId)) {
        qDebug() << "更新图书失败: ISBN已存在";
        return false;
    }
    
    QSqlQuery query(db);
    
    // 先获取当前已借出的数量
    query.prepare("SELECT total_copies - available_copies FROM books WHERE id = ?");
    query.addBindValue(bookId);
    if (!query.exec() || !query.next()) {
        return false;
    }
    int borrowedCopies = query.value(0).toInt();
    
    // 验证总册数不能小于已借出数量
    if (totalCopies < borrowedCopies) {
        qDebug() << "更新图书失败: 总册数不能小于已借出数量";
        return false;
    }
    
    // 更新图书信息
    query.prepare("UPDATE books SET isbn = ?, title = ?, author = ?, publisher = ?, "
                  "publish_date = ?, total_copies = ?, available_copies = ? WHERE id = ?");
    query.addBindValue(isbn);
    query.addBindValue(title);
    query.addBindValue(author);
    query.addBindValue(publisher);
    query.addBindValue(publishDate.toString("yyyy-MM-dd"));
    query.addBindValue(totalCopies);
    query.addBindValue(qMax(0, totalCopies - borrowedCopies));
    query.addBindValue(bookId);
    
    if (!query.exec()) {
        qDebug() << "更新图书失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::hasBookBorrows(int bookId)
{
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM borrows WHERE book_id = ?");
    query.addBindValue(bookId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool Database::isBookIsbnExists(const QString &isbn, int excludeBookId)
{
    QSqlQuery query(db);
    if (excludeBookId > 0) {
        query.prepare("SELECT COUNT(*) FROM books WHERE isbn = ? AND id != ?");
        query.addBindValue(isbn);
        query.addBindValue(excludeBookId);
    } else {
        query.prepare("SELECT COUNT(*) FROM books WHERE isbn = ?");
        query.addBindValue(isbn);
    }
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool Database::deleteBook(int bookId)
{
    // 检查是否有借阅记录
    if (hasBookBorrows(bookId)) {
        qDebug() << "删除图书失败: 该图书存在借阅记录";
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM books WHERE id = ?");
    query.addBindValue(bookId);
    
    if (!query.exec()) {
        qDebug() << "删除图书失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::addReader(const QString &readerId, const QString &name,
                         const QString &phone, const QString &email)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO readers (reader_id, name, phone, email) VALUES (?, ?, ?, ?)");
    query.addBindValue(readerId);
    query.addBindValue(name);
    query.addBindValue(phone);
    query.addBindValue(email);
    
    if (!query.exec()) {
        qDebug() << "添加读者失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::hasReaderBorrows(int readerId)
{
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM borrows WHERE reader_id = ?");
    query.addBindValue(readerId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool Database::isReaderIdExists(const QString &readerId, int excludeReaderId)
{
    QSqlQuery query(db);
    if (excludeReaderId > 0) {
        query.prepare("SELECT COUNT(*) FROM readers WHERE reader_id = ? AND id != ?");
        query.addBindValue(readerId);
        query.addBindValue(excludeReaderId);
    } else {
        query.prepare("SELECT COUNT(*) FROM readers WHERE reader_id = ?");
        query.addBindValue(readerId);
    }
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool Database::updateReader(int readerId, const QString &readerIdStr, const QString &name,
                            const QString &phone, const QString &email)
{
    // 检查读者编号是否与其他读者冲突
    if (isReaderIdExists(readerIdStr, readerId)) {
        qDebug() << "更新读者失败: 读者编号已存在";
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare("UPDATE readers SET reader_id = ?, name = ?, phone = ?, email = ? WHERE id = ?");
    query.addBindValue(readerIdStr);
    query.addBindValue(name);
    query.addBindValue(phone);
    query.addBindValue(email);
    query.addBindValue(readerId);
    
    if (!query.exec()) {
        qDebug() << "更新读者失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::deleteReader(int readerId)
{
    // 检查是否有借阅记录
    if (hasReaderBorrows(readerId)) {
        qDebug() << "删除读者失败: 该读者存在借阅记录";
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM readers WHERE id = ?");
    query.addBindValue(readerId);
    
    if (!query.exec()) {
        qDebug() << "删除读者失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::borrowBook(int bookId, int readerId, const QDate &borrowDate, const QDate &returnDate)
{
    QSqlQuery query(db);
    
    db.transaction();
    
    // 检查是否有可借阅的副本
    query.prepare("SELECT available_copies FROM books WHERE id = ?");
    query.addBindValue(bookId);
    if (!query.exec() || !query.next()) {
        db.rollback();
        return false;
    }
    
    int availableCopies = query.value(0).toInt();
    if (availableCopies <= 0) {
        db.rollback();
        return false;
    }
    
    // 插入借阅记录
    query.prepare("INSERT INTO borrows (book_id, reader_id, borrow_date, return_date, status) "
                  "VALUES (?, ?, ?, ?, 'borrowed')");
    query.addBindValue(bookId);
    query.addBindValue(readerId);
    query.addBindValue(borrowDate.toString("yyyy-MM-dd"));
    query.addBindValue(returnDate.toString("yyyy-MM-dd"));
    
    if (!query.exec()) {
        db.rollback();
        qDebug() << "添加借阅记录失败:" << query.lastError().text();
        return false;
    }
    
    // 更新可用副本数
    query.prepare("UPDATE books SET available_copies = available_copies - 1 WHERE id = ?");
    query.addBindValue(bookId);
    
    if (!query.exec()) {
        db.rollback();
        return false;
    }
    
    db.commit();
    return true;
}

Database::BorrowInfo Database::getBorrowInfo(int borrowId)
{
    BorrowInfo info;
    info.borrowId = borrowId;
    info.isOverdue = false;
    info.overdueDays = 0;
    
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT br.id, b.title, r.name, br.borrow_date, br.return_date,
               CASE WHEN br.return_date < DATE('now') THEN 1 ELSE 0 END as is_overdue,
               CAST(julianday('now') - julianday(br.return_date) AS INTEGER) as overdue_days
        FROM borrows br
        JOIN books b ON br.book_id = b.id
        JOIN readers r ON br.reader_id = r.id
        WHERE br.id = ? AND br.status = 'borrowed'
    )");
    query.addBindValue(borrowId);
    
    if (query.exec() && query.next()) {
        info.borrowId = query.value(0).toInt();
        info.bookTitle = query.value(1).toString();
        info.readerName = query.value(2).toString();
        info.borrowDate = QDate::fromString(query.value(3).toString(), "yyyy-MM-dd");
        info.returnDate = QDate::fromString(query.value(4).toString(), "yyyy-MM-dd");
        info.isOverdue = query.value(5).toInt() > 0;
        info.overdueDays = query.value(6).toInt();
    }
    
    return info;
}

bool Database::returnBook(int borrowId)
{
    QSqlQuery query(db);
    
    db.transaction();
    
    // 获取借阅信息
    query.prepare("SELECT book_id FROM borrows WHERE id = ? AND status = 'borrowed'");
    query.addBindValue(borrowId);
    if (!query.exec() || !query.next()) {
        db.rollback();
        return false;
    }
    
    int bookId = query.value(0).toInt();
    
    // 更新借阅记录
    query.prepare("UPDATE borrows SET status = 'returned', actual_return_date = DATE('now') WHERE id = ?");
    query.addBindValue(borrowId);
    
    if (!query.exec()) {
        db.rollback();
        qDebug() << "更新借阅记录失败:" << query.lastError().text();
        return false;
    }
    
    // 更新可用副本数
    query.prepare("UPDATE books SET available_copies = available_copies + 1 WHERE id = ?");
    query.addBindValue(bookId);
    
    if (!query.exec()) {
        db.rollback();
        qDebug() << "更新图书库存失败:" << query.lastError().text();
        return false;
    }
    
    db.commit();
    return true;
}

QList<QPair<int, QDate>> Database::getOverdueBorrows()
{
    QList<QPair<int, QDate>> overdueList;
    QSqlQuery query(db);
    
    query.prepare("SELECT id, return_date FROM borrows WHERE status = 'borrowed' AND return_date < DATE('now')");
    if (query.exec()) {
        while (query.next()) {
            int borrowId = query.value(0).toInt();
            QDate returnDate = QDate::fromString(query.value(1).toString(), "yyyy-MM-dd");
            overdueList.append(qMakePair(borrowId, returnDate));
        }
    }
    
    return overdueList;
}
