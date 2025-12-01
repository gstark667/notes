#include "Syncer.h"
#include "qwebdav.h"
#include <qnetworkreply.h>
#include <qobject.h>

Syncer::Syncer(QObject *parent): QObject(parent) {
    connect(&mParser, SIGNAL(finished()), this, SLOT(finished()));
    connect(&mParser, SIGNAL(errorChanged(QString)), this, SLOT(error(QString)));
    connect(&mWebdav, SIGNAL(errorChanged(QString)), this, SLOT(error(QString)));
}

void Syncer::open(QString url, QString username, QString password) {
    mUrl = url;
    mUsername = username;
    mPassword = password;

    QUrl qurl(url);
    mRootPath = qurl.path();

    qDebug() << "Syncer: open " << qurl.scheme() << " " << qurl.host() << " " << qurl.path();

    auto connType = QWebdav::HTTPS;
    if (qurl.scheme() == "http") {
        connType = QWebdav::HTTP;
    }

    mWebdav.setConnectionSettings(connType, qurl.host(), qurl.path(), mUsername, mPassword, 443);
}

void Syncer::sync() {
    mParser.listDirectory(&mWebdav, "/", true);
}

void Syncer::finished() {
    qDebug() << "Syncer: connected";

    QList<QWebdavItem> list = mParser.getList();
    QWebdavItem item;
    foreach(item, list) {
        qDebug() << item.name() << " " << item.path() << item.lastModifiedStr();
        QString remotePath = item.path();
        // I don't like this, but slice the preceeding / from the path
        if (remotePath[0] == '/') {
            remotePath = remotePath.slice(1);
        }
        QDateTime remoteLastModified = item.lastModified();
        remoteLastModified.setTimeZone(QTimeZone::UTC);
        QString localPath = QDir(mLocalRoot).filePath(remotePath);
        QFileInfo fileInfo(localPath);
        QDateTime localLastModified = QDateTime::fromSecsSinceEpoch(fileInfo.lastModified().toSecsSinceEpoch());
        // check modified times
        // TODO: store last sync time to detect conflicts when both were edited
        qDebug() << localPath << remoteLastModified << fileInfo.lastModified();
        if (remoteLastModified > localLastModified) {
            // pull remote file
            qDebug() << "pulling remote" << item.path();
            QNetworkReply *reply = mWebdav.get(item.path());
            connect(reply, SIGNAL(readyRead()), this, SLOT(itemRead()));
        } else if (remoteLastModified < localLastModified) {
            // push local file to remote
            QFile file(localPath);
            qDebug() << "pushing local" << item.path() << fileInfo.lastModified();
            if (file.open(QIODevice::ReadOnly)) {
                /*QWebdav::PropValues props;
                QMap<QString, QVariant> davProps;
                davProps["lastmodified"] = 1712426039;
                props["DAV:"] = davProps;*/

                //qDebug() << "test time" << myDateTime;
                QNetworkReply *reply = mWebdav.put(item.path(), file.readAll(), localLastModified);
                //QNetworkReply *reply = mWebdav.proppatch(item.path(), props);
                connect(reply, SIGNAL(finished()), this, SLOT(itemWritten()));
            }
        }
    }
}

QString Syncer::toLocalPath(QString remotePath) {
    QString relativePath = QDir(mRootPath).relativeFilePath(remotePath);
    return QDir(mLocalRoot).filePath(relativePath);
}

void Syncer::itemRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (reply == 0)
        return;

    QString localPath = toLocalPath(reply->url().path());
    QFileInfo info(localPath);
    QDir parentDir = info.dir();
    bool fileWasCreated = false;

    if (!parentDir.exists()) {
        parentDir.mkpath(".");
    }
    fileWasCreated = !info.exists();

    QDateTime lastModified = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();

    QFile file(localPath);
    // remote we updated more recently than local, pull the file
    // TODO: store last sync time to detect conflicts when both were edited
    // TODO: do this in the list section if possible?
    qDebug() << "getting" << localPath << lastModified << info.lastModified();
    if (info.lastModified() < lastModified) {
        if (file.open(QIODevice::WriteOnly)) {// | QIODevice::Text)) {
            file.write(reply->readAll());
            qDebug() << "setting file time" << localPath << lastModified << info.lastModified();
            if (!file.setFileTime(lastModified, QFileDevice::FileModificationTime)) {
                qDebug() << "failed to update time";
            }
            file.close();
        } else {
            qWarning("Failed to open file: %s", qUtf8Printable(localPath));
            fileWasCreated = false;
        }
    }

    if (fileWasCreated) {
        emit fileCreated(QDir(mRootPath).relativeFilePath(reply->url().path()));
    }
}

void Syncer::itemWritten() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (reply == 0)
        return;
    qDebug() << "itemWritten" << reply->url().path() << reply->readAll();
}

void Syncer::error(QString message) {
    qWarning() << "Syncer: " << message;
}
