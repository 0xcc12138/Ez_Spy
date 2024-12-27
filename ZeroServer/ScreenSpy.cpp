#include "ScreenSpy.h"
ScreenSpy::ScreenSpy(QWidget* parent) :
    QWidget(parent), mScreenLen(0)
{
    // ��ʼ������
    const int w = 800, h = 450;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - h) >> 1;
    this->setGeometry(x, y, w, h);
    // ͼƬlabel
    mScreenLabel = new QLabel(this);
    mScreenLabel->setGeometry(0, 0, w, h);
    mScreenLabel->setScaledContents(true);
}
int ScreenSpy::startScreenSpyServer(QString userName)
{
    // ���ô��ڱ���
    this->setWindowTitle(userName.append("-��Ļ���"));
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
void ScreenSpy::newConnection(QTcpSocket* s)
{
    // �����ͻ�
    mSock = new TcpSocket(s, this);
    connect(mSock, SIGNAL(newData()), this, SLOT(processBuffer()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    // ���ټ����¿ͻ�
    mServer->server()->close();
}
void ScreenSpy::processBuffer()
{
    QByteArray* buf = mSock->buffer();
    while ((unsigned int)buf->size() >= sizeof(ScreenSpyHeader)) {
        // ��ȡͷ
        if (mScreenLen == 0) {
            if ((unsigned int)buf->size() >= sizeof(ScreenSpyHeader)) {
                ScreenSpyHeader header;
                memcpy(&header, buf->data(), sizeof(ScreenSpyHeader));
                buf->remove(0, sizeof(ScreenSpyHeader));
                // ����ͼƬ����
                mScreenLen = header.len;
            }
            else {
                break;
            }
        }
        // ��ȡJPGͼƬ
        if (mScreenLen > 0) {
            if ((unsigned int)buf->size() >= mScreenLen) {
                // ������ͼƬ
                mScreenPixmap.loadFromData((unsigned char*)buf->data(), mScreenLen);
                // ��ʾͼƬ
                mScreenLabel->setPixmap(mScreenPixmap);
                // ��������
                buf->remove(0, mScreenLen);
                mScreenLen = 0;
            }
            else {
                break;
            }
        }
    }
}
void ScreenSpy::resizeEvent(QResizeEvent*)
{
    // ����mScreenLabel��С
    mScreenLabel->setGeometry(0, 0, width(), height());
}
void ScreenSpy::closeEvent(QCloseEvent*)
{
    // ɾ������
    deleteLater();
}