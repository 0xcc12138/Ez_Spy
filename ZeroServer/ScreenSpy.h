#pragma once

#ifndef SCREENSPY_H
#define SCREENSPY_H

#include <QWidget>
#include <tcpsocket.h>
#include <tcpserver.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <QLabel>

class ScreenSpy : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenSpy(QWidget* parent = 0);

    // ����ͷ
    typedef struct {
        unsigned int len;    // jpg�ļ���С
    } ScreenSpyHeader;


    // ��ʼ��ط�������Ȼ�󷵻��µĶ˿ں�
    int startScreenSpyServer(QString userName);

private:
    QLabel* mScreenLabel;        // ������ʾͼƬ
    TcpServer* mServer;   // ��Ļ��ط����
    TcpSocket* mSock;   // ��Ļ��ؿͻ���
    QPixmap mScreenPixmap;    // ��ǰ��Ļ��ͼ
    unsigned int mScreenLen;    // JPGͼƬ��С

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

#endif // SCREENSPY_H
