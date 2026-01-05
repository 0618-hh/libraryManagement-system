# 图书借阅管理系统

基于Qt和SQLite的图书与借阅管理系统，采用Model/View架构实现。

## 功能特性

### 核心功能
- ✅ **图书信息管理**：录入、修改、删除图书信息，支持ISBN、书名、作者、出版社等信息
- ✅ **读者档案管理**：管理读者信息，包括读者编号、姓名、联系方式等
- ✅ **借还书流程**：完整的借书和还书流程，自动管理图书库存
- ✅ **数据统计**：统计图书总数、读者总数、借阅情况等
- ✅ **多条件筛选查询**：支持按ISBN、书名、作者等条件筛选图书；按读者编号、姓名筛选读者；按图书名称、读者姓名、状态筛选借阅记录
- ✅ **逾期提醒**：自动检测逾期借阅，通过系统托盘图标和消息框提醒用户

### 技术特点
- 使用SQLite数据库存储数据
- 采用Qt的Model/View架构，数据与视图分离
- 使用QSqlTableModel和QSqlRelationalTableModel实现数据模型
- 支持事务处理，确保数据一致性
- 自动管理图书可借册数

## 编译和运行

### 环境要求
- Qt 5.12 或更高版本
- C++17 编译器
- SQLite（Qt自带）

### 编译步骤

1. 使用Qt Creator打开 `Rent.pro` 文件

2. 或者使用命令行编译：
```bash
qmake Rent.pro
make  # Windows上使用: nmake 或 mingw32-make
```

3. 运行程序：
```bash
./Rent  # Windows上: Rent.exe
```

## 数据库结构

### books 表（图书表）
- id: 主键
- isbn: ISBN号（唯一）
- title: 书名
- author: 作者
- publisher: 出版社
- publish_date: 出版日期
- total_copies: 总册数
- available_copies: 可借册数
- created_at: 创建时间

### readers 表（读者表）
- id: 主键
- reader_id: 读者编号（唯一）
- name: 姓名
- phone: 电话
- email: 邮箱
- created_at: 创建时间

### borrows 表（借阅记录表）
- id: 主键
- book_id: 图书ID（外键）
- reader_id: 读者ID（外键）
- borrow_date: 借阅日期
- return_date: 应还日期
- actual_return_date: 实际归还日期
- status: 状态（borrowed/returned）
- created_at: 创建时间

## 使用说明

### 图书管理
1. 在"图书管理"标签页添加、修改或删除图书
2. 使用搜索功能按ISBN、书名或作者筛选图书
3. 可借册数为0的图书会以红色显示

### 读者管理
1. 在"读者管理"标签页管理读者信息
2. 支持按读者编号或姓名搜索

### 借还书管理
1. 输入图书ID和读者ID进行借书操作
2. 设置借阅天数，系统自动计算应还日期
3. 在借阅记录中选择已借出的记录进行还书操作
4. 逾期记录会以红色高亮显示

### 数据统计
1. 查看图书总数、读者总数、借阅总数和逾期数量
2. 查看热门图书排行榜（按借阅次数）
3. 查看活跃读者排行榜
4. 查看所有逾期借阅记录及逾期天数

### 逾期提醒
- 系统每小时自动检查一次逾期借阅
- 发现逾期时会在系统托盘显示通知
- 如果窗口可见，也会弹出消息框提醒

## 项目结构

```
Rent/
├── main.cpp                 # 程序入口
├── mainwindow.h/cpp/ui      # 主窗口
├── database.h/cpp           # 数据库管理类（单例模式）
├── bookmodel.h/cpp          # 图书数据模型
├── readermodel.h/cpp        # 读者数据模型
├── borrowmodel.h/cpp        # 借阅数据模型
├── bookwidget.h/cpp/ui      # 图书管理界面
├── readerwidget.h/cpp/ui    # 读者管理界面
├── borrowwidget.h/cpp/ui    # 借还书管理界面
├── statisticswidget.h/cpp/ui # 数据统计界面
├── Rent.pro                 # Qt项目文件
└── README.md                # 说明文档
```

## 注意事项

1. 数据库文件会自动创建在应用程序数据目录
2. 删除图书或读者前，请确保没有相关的借阅记录
3. 借书时会自动检查图书库存，无库存时无法借出
4. 还书时会自动更新图书的可借册数

## 开发说明

本项目采用以下设计模式：
- **单例模式**：Database类使用单例模式确保数据库连接唯一
- **Model/View架构**：使用Qt的Model/View框架实现数据与视图分离
- **事务处理**：借还书操作使用数据库事务确保数据一致性

## 许可证

本项目仅供学习和教育使用。
