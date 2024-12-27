#pragma once
#ifndef FILETRANSFER_H
#define FILETRANSFER_H


#include <QObject>
#include <QProgressDialog>
#include "TcpServer.h"
#include "TcpSocket.h"
#include <QFile>
#include <QDir>


class FileTransfer : public QObject
{
    Q_OBJECT
public:
    explicit FileTransfer(QObject* parent = 0);


    // �ļ����ݰ�ͷ
    typedef struct {
        char fileName[256];
        unsigned int len;
    } FileHeader;


    // ��ʼ�ļ����շ�������Ȼ�󷵻��µĶ˿ں�
    int startRecvFileServer(QString userName, QString filePath);


    // ��ʼ�ļ����ͷ�������Ȼ�󷵻��µĶ˿ں�
    int startSendFileServer(QString userName, QString filePath);


private:
    QProgressDialog* mProgress; // ����������
    TcpServer* mServer;     // �����
    TcpSocket* mSock;     // �ͻ���
    QFile mFile;        // �ļ�
    FileHeader mHeader; // �ļ�ͷ
    unsigned int _curWritten;       // �Ѿ�д������


signals:


public slots:
    void close();


    // �û�����
    void newRecvFileConnection(QTcpSocket*);
    void newSendFileConnection(QTcpSocket*);


    // �����ļ�����
    void recvData();


    // ��������
    void sendData();
};


#endif // FILETRANSFER_H
