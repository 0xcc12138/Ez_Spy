#include "m_Window.h"



void m_Window::InitServer()
{
    //emit clientLogin(id, userName, ip, port, system);
    mZeroServer = new ZeroServer(this);
    connect(mZeroServer, SIGNAL(clientLogin(int, QString, QString, int, QString)),this, SLOT(addClientToTable(int, QString, QString, int, QString)));
    connect(mZeroServer, SIGNAL(clientLogout(int)), this, SLOT(removeClientFromTable(int)));
    mZeroServer->start(18000);
}


void m_Window::addClientToTable(int ID, QString Username, QString IP, int port, QString System_version)
{
    // 获取当前行数
    int rowCount = ui.tableWidget->rowCount();

    // 插入新行
    ui.tableWidget->insertRow(rowCount);

    // 添加数据到新行的各个单元格
    ui.tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(ID)));  // 第1列 int 类型
    ui.tableWidget->setItem(rowCount, 1, new QTableWidgetItem(Username));  // 第2列 QString 类型
    ui.tableWidget->setItem(rowCount, 2, new QTableWidgetItem(IP));  // 第3列 QString 类型
    ui.tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(port)));  // 第4列 int 类型
    ui.tableWidget->setItem(rowCount, 4, new QTableWidgetItem(System_version));  // 第5列 QString 类型
}

void m_Window::removeClientFromTable(int ID)
{
    // 获取表格中的行数
    int rowCount = ui.tableWidget->rowCount();

    // 遍历每一行，查找匹配的ID
    for (int row = 0; row < rowCount; ++row)
    {
        // 获取当前行第0列的 ID（假设 ID 存储在第0列）
        int currentID = ui.tableWidget->item(row, 0)->text().toInt();

        // 如果找到匹配的 ID，则删除该行
        if (currentID == ID)
        {
            ui.tableWidget->removeRow(row);
            break;  // 找到并删除后退出循环
        }
    }
}

void m_Window::Initialize_Interface()
{
    

    Set_visual_control();//初始化控件的位置

    
    Set_Right_click_options();//给表单结构设置右键选项

    InitServer(); //初始化服务端，并启动

}


void m_Window::Set_Right_click_options()//给表单结构的行设置右键选项
{
    // 设置布局
    setCentralWidget(ui.tableWidget);

    // 启用自定义右键菜单
    ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // 连接信号和槽函数
    connect(ui.tableWidget, &QTableWidget::customContextMenuRequested,
        this, &m_Window::showContextMenu);
}



void m_Window::showContextMenu(const QPoint& pos) {
    // 创建右键菜单
    QMenu contextMenu(this);

    // 创建菜单项
    QAction* sendMessageAction = contextMenu.addAction("发送弹窗消息");
    QAction* rebootAction = contextMenu.addAction("重新开机");
    QAction* forceLogoutAction = contextMenu.addAction("强制下线");

    // 显示菜单并获取用户选择的动作
    QAction* selectedAction = contextMenu.exec(ui.tableWidget->mapToGlobal(pos));

    // 执行菜单项操作
    if (selectedAction == sendMessageAction) {
        QMessageBox::information(this, "弹窗消息", "你选择了发送弹窗消息");
    }
    else if (selectedAction == rebootAction) {
        QMessageBox::information(this, "重新开机", "你选择了重新开机");
        // 这里可以添加系统重启的代码
    }
    else if (selectedAction == forceLogoutAction) {
        QMessageBox::information(this, "强制下线", "你选择了强制下线");
        // 这里可以添加强制下线的代码
    }
}



void m_Window::Set_visual_control()
{
    // 设置 QLabel 的固定宽度
    ui.label->setFixedWidth(100);  // 设置 QLabel 的宽度为 100 像素
    ui.label->setAlignment(Qt::AlignCenter);  // 设置文字居中（可选）
    ui.toolBar->setFixedHeight(100);  // 将工具栏的高度设置为 50 像素


    // 设置按钮大小策略为 Expanding
    ui.pushButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui.pushButton_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui.pushButton_3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui.pushButton_4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui.label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    // 将按钮添加到工具栏
    ui.toolBar->addWidget(ui.label);
    ui.toolBar->addWidget(ui.pushButton);
    ui.toolBar->addWidget(ui.pushButton_2);
    ui.toolBar->addWidget(ui.pushButton_3);
    ui.toolBar->addWidget(ui.pushButton_4);


    // 设置列标题
    QStringList horizontalHeader;
    horizontalHeader << "ID" << "用户名" << "IP" << "端口" << "系统";
    ui.tableWidget->setHorizontalHeaderLabels(horizontalHeader);

    // 设置行标题
    QStringList verticalHeader;
    verticalHeader << "行1" << "行2" << "行3" << "行4";
    ui.tableWidget->setVerticalHeaderLabels(verticalHeader);

    // 设置列宽自适应并均匀分布
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 使用 Stretch 模式
}
