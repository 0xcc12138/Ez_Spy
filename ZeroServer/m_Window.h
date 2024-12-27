
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
        ui.setupUi(this); // ��ʼ������
        Initialize_Interface();

        // �����źźͲ�
        connect(ui.pushButton_6, &QPushButton::clicked, this, &m_Window::onCreateServerButtonClicked);

        connect(ui.pushButton_5, &QPushButton::clicked, this, &m_Window::onStopServerButtonClicked);

    }

    ~m_Window()
    {}


private:
    Ui::ServerClass ui; // ������ .ui �ļ����ɵ� UI ��
    ZeroServer* mZeroServer; //����˹������

private:
    void Initialize_Interface();    //��ʼ������
    void Set_Right_click_options(); //�����ṹ���������Ҽ�ѡ��
    void Set_visual_control();  //�����ؼ�λ�õĳ�ʼ��

    void InitServer(unsigned int port);
    void StopServer();


private slots:
    void showContextMenu(const QPoint& pos);

    void addClientToTable(int, QString, QString, int, QString);

    void removeClientFromTable(int);

    void onCreateServerButtonClicked(); // ��������˰�ť����ۺ���

    void onStopServerButtonClicked(); // ��������˰�ť����ۺ���

};