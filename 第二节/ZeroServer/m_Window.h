#include "Server_ui.h"
#include <QMessageBox>
#include <QMenu>
#include "ZeroServer.h"
#pragma execution_character_set("utf-8")

class m_Window : public QMainWindow {
    Q_OBJECT

public:
    explicit m_Window(QWidget* parent = nullptr)
        : QMainWindow(parent) {
        ui.setupUi(this); // 初始化界面
        Initialize_Interface();
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

    void InitServer();



private slots:
    void showContextMenu(const QPoint& pos);

    void addClientToTable(int, QString, QString, int, QString);

    void removeClientFromTable(int);
};