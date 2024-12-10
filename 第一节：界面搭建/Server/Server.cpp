#include "Server.h"




Server::Server(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    Initialize_Interface(); //初始化界面

}

Server::~Server()
{}


void Server::Initialize_Interface()
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


    //给表单结构设置右键选项
    Set_Right_click_options();

}


void Server::Set_Right_click_options()//给表单结构的行设置右键选项
{
    // 设置布局
    setCentralWidget(ui.tableWidget);

    // 启用自定义右键菜单
    ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // 连接信号和槽函数
    connect(ui.tableWidget, &QTableWidget::customContextMenuRequested,
        this, &Server::showContextMenu);
}

void Server::showContextMenu(const QPoint& pos) {
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


