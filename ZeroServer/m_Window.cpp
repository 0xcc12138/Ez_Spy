#include "m_Window.h"



void m_Window::InitServer(unsigned int prot)
{
    //emit clientLogin(id, userName, ip, port, system);
    mZeroServer = new ZeroServer(this);
    connect(mZeroServer, SIGNAL(clientLogin(int, QString, QString, int, QString)),this, SLOT(addClientToTable(int, QString, QString, int, QString)));
    connect(mZeroServer, SIGNAL(clientLogout(int)), this, SLOT(removeClientFromTable(int)));
    mZeroServer->start(prot);
}

void m_Window::StopServer()
{
    mZeroServer->stop();
}


void m_Window::addClientToTable(int ID, QString Username, QString IP, int port, QString System_version)
{
    // ��ȡ��ǰ����
    int rowCount = ui.tableWidget->rowCount();

    // ��������
    ui.tableWidget->insertRow(rowCount);

    // ������ݵ����еĸ�����Ԫ��
    ui.tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(ID)));  // ��1�� int ����
    ui.tableWidget->setItem(rowCount, 1, new QTableWidgetItem(Username));  // ��2�� QString ����
    ui.tableWidget->setItem(rowCount, 2, new QTableWidgetItem(IP));  // ��3�� QString ����
    ui.tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(port)));  // ��4�� int ����
    ui.tableWidget->setItem(rowCount, 4, new QTableWidgetItem(System_version));  // ��5�� QString ����
}

void m_Window::removeClientFromTable(int ID)
{
    // ��ȡ����е�����
    int rowCount = ui.tableWidget->rowCount();

    // ����ÿһ�У�����ƥ���ID
    for (int row = 0; row < rowCount; ++row)
    {
        // ��ȡ��ǰ�е�0�е� ID������ ID �洢�ڵ�0�У�
        int currentID = ui.tableWidget->item(row, 0)->text().toInt();

        // ����ҵ�ƥ��� ID����ɾ������
        if (currentID == ID)
        {
            ui.tableWidget->removeRow(row);
            break;  // �ҵ���ɾ�����˳�ѭ��
        }
    }
}

void m_Window::Initialize_Interface()
{
    

    Set_visual_control();//��ʼ���ؼ���λ��

    
    Set_Right_click_options();//�����ṹ�����Ҽ�ѡ��

    

}


void m_Window::Set_Right_click_options()//�����ṹ���������Ҽ�ѡ��
{
    // �����Զ����Ҽ��˵�
    ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // �����źźͲۺ���
    connect(ui.tableWidget, &QTableWidget::customContextMenuRequested,
        this, &m_Window::showContextMenu);
}



void m_Window::showContextMenu(const QPoint& pos) {
    // �����Ҽ��˵�
    QMenu contextMenu(this);

    // �����˵���
    QAction* sendMessageAction = contextMenu.addAction("���͵�����Ϣ");
    QAction* rebootAction = contextMenu.addAction("���¿���");
    QAction* forceLogoutAction = contextMenu.addAction("ǿ������");

    // ��ʾ�˵�����ȡ�û�ѡ��Ķ���
    QAction* selectedAction = contextMenu.exec(ui.tableWidget->mapToGlobal(pos));

    // ִ�в˵������
    if (selectedAction == sendMessageAction) {
        QMessageBox::information(this, "������Ϣ", "��ѡ���˷��͵�����Ϣ");
    }
    else if (selectedAction == rebootAction) {
        QMessageBox::information(this, "���¿���", "��ѡ�������¿���");
        // ����������ϵͳ�����Ĵ���
    }
    else if (selectedAction == forceLogoutAction) {
        QMessageBox::information(this, "ǿ������", "��ѡ����ǿ������");
        // ����������ǿ�����ߵĴ���
    }
}



void m_Window::Set_visual_control()
{
    // ���� QLabel �Ĺ̶����
    ui.label->setFixedWidth(100);  // ���� QLabel �Ŀ��Ϊ 100 ����
    ui.label->setAlignment(Qt::AlignCenter);  // �������־��У���ѡ��
    ui.toolBar->setFixedHeight(100);  // ���������ĸ߶�����Ϊ 50 ����


    // ���ð�ť��С����Ϊ Expanding
    ui.pushButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui.pushButton_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui.pushButton_3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui.pushButton_4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui.label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    // ����ť��ӵ�������
    ui.toolBar->addWidget(ui.label);
    ui.toolBar->addWidget(ui.pushButton);
    ui.toolBar->addWidget(ui.pushButton_2);
    ui.toolBar->addWidget(ui.pushButton_3);
    ui.toolBar->addWidget(ui.pushButton_4);



    // ���ñ༭��Ͱ�ť�Ĺ̶���С
    ui.lineEdit->setFixedHeight(30);  // ���� QLineEdit �߶ȹ̶�Ϊ 30 ����
    ui.lineEdit->setFixedWidth(100); // ���� QLineEdit ��ȹ̶�Ϊ 200 ����

    ui.pushButton_5->setFixedSize(100, 30);  // ���� QPushButton �̶���С
    ui.pushButton_6->setFixedSize(100, 30);  // ���� QPushButton �̶���С

    // ����һ���µĲ��֣��������������ؼ�
    QVBoxLayout* fixedLayout = new QVBoxLayout();

    // ���ؼ���ӵ��ò�����
    fixedLayout->addWidget(ui.lineEdit);
    fixedLayout->addWidget(ui.pushButton_5);
    fixedLayout->addWidget(ui.pushButton_6);

    // �����������ؼ��ļ��Ϊ�̶�ֵ
    fixedLayout->setSpacing(10);  // �ؼ�֮��Ĵ�ֱ���̶�Ϊ 10 ����

    // ���Ӳ�����ӵ� QGridLayout ��
    ui.gridLayout->addLayout(fixedLayout, 0, 0, 1, 1);

    // �����б���
    QStringList horizontalHeader;
    horizontalHeader << "ID" << "�û���" << "IP" << "�˿�" << "ϵͳ";
    ui.tableWidget->setHorizontalHeaderLabels(horizontalHeader);

    // �����б���
    QStringList verticalHeader;
    verticalHeader << "��1" << "��2" << "��3" << "��4";
    ui.tableWidget->setVerticalHeaderLabels(verticalHeader);

    // �����п�����Ӧ�����ȷֲ�
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // ʹ�� Stretch ģʽ
}


void m_Window::onCreateServerButtonClicked() // ��ť����ۺ���
{
    QString text = ui.lineEdit->text(); // ��ȡ�ı�����
    bool ok;                        // ת���Ƿ�ɹ��ı�־
    int value = text.toInt(&ok);    // ת��Ϊ����

    if (ok) 
    {
        InitServer(value);
    }
    else {
        qDebug() << "Input is not a valid port!";
    }
}

void m_Window::onStopServerButtonClicked() // ��������˰�ť����ۺ���
{
    StopServer();
}