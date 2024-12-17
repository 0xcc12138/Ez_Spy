
#include <QMessageBox>
#include <QMenu>
#include "ZeroServer.h"
#include "Server_ui.h"
#include <QInputDialog>
#include "ScreenSpy.h"
#pragma execution_character_set("utf-8")

class m_Window : public QMainWindow {
	Q_OBJECT


private:
	void Initialize_Interface();    //初始化界面
	void Set_Right_click_options(); //给表单结构的行设置右键选项
	void Set_visual_control();  //各个控件位置的初始化

	void InitServer(unsigned int port);  //初始化服务端
	void StopServer();  //停止服务端


public:
	explicit m_Window(QWidget* parent = nullptr)
		: QMainWindow(parent) {
		ui.setupUi(this); // 初始化界面
		Initialize_Interface();

		// 连接信号和槽
		connect(ui.pushButton_6, &QPushButton::clicked, this, &m_Window::onCreateServerButtonClicked);

		connect(ui.pushButton_5, &QPushButton::clicked, this, &m_Window::onStopServerButtonClicked);

		connect(ui.pushButton_3, &QPushButton::clicked, this, &m_Window::screenSpyClicked);
	}

	~m_Window()
	{}


private:
	Ui::ServerClass ui; // 包含由 .ui 文件生成的 UI 类
	ZeroServer* mZeroServer; //服务端管理操作


private:
	QPoint m_lastClickPos;  // 用于保存上次右键点击的位置




private slots:
	void showContextMenu(const QPoint& pos, QMenu* contextMenu);

	void addClientToTable(int, QString, QString, int, QString);

	void removeClientFromTable(int);

	void onCreateServerButtonClicked(); // 创建服务端按钮点击槽函数

	void onStopServerButtonClicked(); // 创建服务端按钮点击槽函数

	void onTriggered();//处理右键点击表单选项

	void screenSpyClicked(); // 创建服务端按钮点击槽函数


};