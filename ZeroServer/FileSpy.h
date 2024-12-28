
#ifndef FILESPY_H
#define FILESPY_H

#include <QWidget>
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QListWidget>
#include "tcpserver.h"
#include "tcpsocket.h"
#include <QMenu>
#include <QDir>
#include <QFileIconProvider>
#include <QMessageBox>
#include "filetransfer.h"

class FileSpy : public QWidget
{
    Q_OBJECT
public:
    explicit FileSpy(QWidget* parent = 0);

    // �������ͻ��˷��͵�ָ��(���������Ҫ��Ҳ���������Լ���ָ��)
    const QByteArray CmdGetDirFiles = "GET_DIRS_FILES";   // ��ȡ·���µ������ļ�����·����
    const QByteArray CmdDownloadFile = "DOWNLOAD_FILE";   // ����˴ӿͻ�Ҳ�����ļ�
    const QByteArray CmdUploadFile = "UPLOAD_FILE";       // ������ϴ��ļ����ͻ���
    const QByteArray CmdDeleteFile = "DELETE_FILE";       // ������ڿͻ���ɾ���ļ�

    // �ͻ��������˷��͵�ָ��(���������Ҫ��Ҳ���������Լ���ָ��)
    const QByteArray CmdSendDrives = "SEND_DRIVES";        // �����̷�
    const QByteArray CmdSendDirs = "SEND_DIRS";            // ����·���µ�����·����
    const QByteArray CmdSendFiles = "SEND_FILES";          // ����·���µ������ļ���
    const QByteArray CmdDeleteFileSuccess = "DELETE_SUCCESS";  // �ɹ�ɾ���ļ�
    const QByteArray CmdDeleteFileFailed = "DELETE_FAILED";    // ɾ���ļ�ʧ��

    // �ָ���źͽ������ţ������ȡ�ļ��������ļ�����:FILES<�ָ��>FILEA<�ļ��ָ��>FILEB<�ļ��ָ��>FILEC<��������>
    const QByteArray CmdSplit = ";";
    const QByteArray CmdEnd = "\r\n";
    const QByteArray CmdFileSplit = "|";

    // ��ʼ��ط�������Ȼ�󷵻��µĶ˿ں�
    int startFileSpyServer(QString userName);

private:
    QListWidget* mClientFileList;   // �ͻ����ļ��б�
    QListWidget* mServerFileList;   // �����ļ��б�
    QMenu* mClientMenu;             // �Կͻ����б�Ĳ����˵�
    QMenu* mServerMenu;             // �Ա����б�Ĳ����˵�
    QDir _curClientDir;      // ��ǰ�ͻ�·��
    QDir _curServerDir;      // ��ǰ����·��
    TcpServer* mServer;     // �ļ���ط����
    TcpSocket* mSock;       // �ͻ���
    QString _userName;

    // ���б�Ĳ���
    const QByteArray _dirBack = ".."; // ������һҳ
    void addFilesToList(QListWidget* list, QList<QByteArray> strList, QFileIconProvider::IconType iconType);
    QString getCurrentFile(QListWidget* list);
    QList<QByteArray> getLocalDrives();
    QList<QByteArray> getLocalDirs(QDir dir);
    QList<QByteArray> getLocalFiles(QDir dir);

    // ����ָ��
    void processCommand(QByteArray& cmd, QByteArray& args);
    // �ֽ�ָ��Ĳ��������ع�ϣ��
    QHash<QByteArray, QByteArray> parseArgs(QByteArray& args);
    // ������Ӧ��ָ��
    void doSendDrives(QHash<QByteArray, QByteArray>& args);
    void doSendDirs(QHash<QByteArray, QByteArray>& args);
    void doSendFiles(QHash<QByteArray, QByteArray>& args);

signals:

public slots:
    // ���ؿͻ���Ŀ¼
    void loadClientDir(QListWidgetItem* item);
    // ˢ�¿ͻ��б�
    void refreshClientList();
    // ˢ�¿ͻ��б�
    void refreshClientList(QString path);
    // �����ļ�
    void downloadFile();
    // ɾ���ļ�
    void deleteFile();

    // ���ر���Ŀ¼
    void loadServerDir(QListWidgetItem* item);
    void loadServerDir(QDir dir);
    // ˢ�±����б�
    void refreshServerList();
    // �ϴ������ļ�
    void uploadFile();

    // ���¿ͻ�����
    void newConnection(QTcpSocket* s);
    // ����������
    void processBuffer();

protected:
    // �¼����ˣ���Ҫ������ȡ�����˵��¼�
    bool eventFilter(QObject* watched, QEvent* event);

    // �ر�
    void closeEvent(QCloseEvent*);
};

#endif // FILESPY_H