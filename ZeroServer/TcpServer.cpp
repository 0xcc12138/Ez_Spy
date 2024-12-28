#include "TcpServer.h"
#include <QMessageBox>
TcpServer::TcpServer(QObject* parent) : QObject(parent)
{
	mServer = new QTcpServer(this);
	connect(mServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void TcpServer::start(int port)
{
	if (!mServer->isListening()) {
		//QHostAddress serverAddress("27.25.151.172");  // 使用指定的服务器地址
		if (mServer->listen(QHostAddress::AnyIPv4, port)) {
			qDebug() << "服务端监听成功";
			// 弹出消息框提示监听成功
			QMessageBox::information(nullptr, "监听成功", "服务端监听成功，端口：" + QString::number(port));
		}
		else {
			qDebug() << "服务端监听失败：" << mServer->errorString();
			// 弹出错误消息框
			QMessageBox::critical(nullptr, "监听失败", "服务端监听失败: " + mServer->errorString());
		}
	}
}


void TcpServer::stop()
{
	if (mServer->isListening()) {
		mServer->close();
	}
}

void TcpServer::newConnection()
{
	while (mServer->hasPendingConnections()) {
		// 获取新连接
		QTcpSocket* sock = mServer->nextPendingConnection();
		// 发射新连接信号让调用服务器的类知道
		emit newConnection(sock);
	}
}