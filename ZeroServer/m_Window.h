
#include <QMessageBox>
#include <QMenu>
#include "ZeroServer.h"
#include "Server_ui.h"
#pragma execution_character_set("utf-8")

class m_Window : public QMainWindow {
    Q_OBJECT

public:
    explicit m_Window(QWidget* parent = nullptr)
        : QMainWindow(parent) {
        ui.setupUi(this); // 初始化界面
        Initialize_Interface();

        // 连接信号和槽
        connect(ui.pushButton_6, &QPushButton::clicked, this, &m_Window::onCreateServerButtonClicked);

        connect(ui.pushButton_5, &QPushButton::clicked, this, &m_Window::onStopServerButtonClicked);

    }

    ~m_Window()
    {}


private:
    Ui::ServerClass ui; // 包含由 .ui 文件生成的 UI 类
    ZeroServer* mZeroServer; //服务端管理操作

private:
    void Initialize_Interface();    //初始化界面
    void Set_Right_click_options(); //给表单结构的行设置右键选项
    void Set_visual_control();  //各个控件位置的初始化

    void InitServer(unsigned int port);
    void StopServer();


private slots:
    void showContextMenu(const QPoint& pos);

    void addClientToTable(int, QString, QString, int, QString);

    void removeClientFromTable(int);

    void onCreateServerButtonClicked(); // 创建服务端按钮点击槽函数

    void onStopServerButtonClicked(); // 创建服务端按钮点击槽函数

};