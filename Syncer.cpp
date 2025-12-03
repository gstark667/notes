#include "Syncer.h"
#include "qwebdav.h"
#include <qnetworkreply.h>
#include <qobject.h>
#include <qurl.h>

#include <QtMinMax>

Syncer::Syncer(QObject *parent): QObject(parent) {
    mSettings.beginGroup("syncs");

    connect(&mParser, SIGNAL(finished()), this, SLOT(listFinished()));
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

void Syncer::listFinished() {
    QList<QWebdavItem> list = mParser.getList();
    QList<QString> processed;
    QWebdavItem item;
    foreach(item, list) {
        QString remotePath = item.path();
        // I don't like this, but slice the preceeding / from the path
        if (remotePath[0] == '/') {
            remotePath = remotePath.slice(1);
        }
        QDateTime remoteLastModified = item.lastModified();
        remoteLastModified.setTimeZone(QTimeZone::UTC);
        QString localPath = mLocalRoot.filePath(remotePath);
        QFileInfo fileInfo(localPath);
        QDateTime localLastModified = QDateTime::fromSecsSinceEpoch(fileInfo.lastModified().toSecsSinceEpoch());
        qint64 syncTime = mSettings.value(remotePath, 0).toLongLong();
        // check modified times
        // TODO: store last sync time to detect conflicts when both were edited
        qDebug() << localPath << remoteLastModified << fileInfo.lastModified();
        // we need to pull the remote file if it's newer or we have a merge conflict
        if (remoteLastModified > localLastModified || (
                remoteLastModified.toSecsSinceEpoch() > syncTime &&
                localLastModified.toSecsSinceEpoch() > syncTime)) {
            // pull remote file
            QNetworkReply *reply = mWebdav.get(item.path());
            connect(reply, SIGNAL(readyRead()), this, SLOT(itemRead()));
        } else if (remoteLastModified < localLastModified) {
            putFile(localPath);
        }
        processed.push_back(remotePath);
    }

    // sync anything local that wasn't on the server
    QDirIterator it(mLocalRoot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        QString relativePath = mLocalRoot.relativeFilePath(path);
        QFileInfo info(path);
        if (info.isDir() || processed.contains(relativePath)) {
            continue;
        }
        qDebug() << "uploading" << path;
        putFile(path);
    }
}

void Syncer::putFile(QString path) {
    QFileInfo info(path);
    QFile file(path);
    QString remotePath = mLocalRoot.relativeFilePath(path);
    if (file.open(QIODevice::ReadOnly)) {
        /*QWebdav::PropValues props;
        QMap<QString, QVariant> davProps;
        davProps["lastmodified"] = 1712426039;
        props["DAV:"] = davProps;*/

        //qDebug() << "test time" << myDateTime;
        QNetworkReply *reply = mWebdav.put("/" + remotePath, file.readAll(), info.lastModified());
        mSettings.setValue(remotePath, info.lastModified().toSecsSinceEpoch());
        //QNetworkReply *reply = mWebdav.proppatch(item.path(), props);
        connect(reply, SIGNAL(finished()), this, SLOT(itemWritten()));
    }
}

QString Syncer::toLocalPath(QString remotePath) {
    QString relativePath = QDir(mRootPath).relativeFilePath(remotePath);
    return mLocalRoot.filePath(relativePath);
}

void Syncer::itemRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (reply == 0)
        return;

    QString relativePath = QDir(mRootPath).relativeFilePath(reply->url().path());
    QString localPath = mLocalRoot.filePath(relativePath);
    QFileInfo info(localPath);
    QDir parentDir = info.dir();
    QByteArray remoteData = reply->readAll();
    bool fileWasCreated = false;

    if (!parentDir.exists()) {
        parentDir.mkpath(".");
    }
    fileWasCreated = !info.exists();

    QDateTime lastModified = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();

    qint64 syncTime = mSettings.value(relativePath, 0).toLongLong();

    QFile file(localPath);
    // remote we updated more recently than local, pull the f45ile
    // TODO: store last sync time to detect conflicts when both were edited
    // TODO: do this in the list section if possible?
    qDebug() << "getting" << localPath << lastModified << info.lastModified();
    // if (info.lastModified().toSecsSinceEpoch() > syncTime &&
    //     lastModified.toSecsSinceEpoch() > syncTime) {
    //         qDebug() << "merge conflict possible" << localPath;
    if (info.lastModified().toSecsSinceEpoch() > syncTime &&
            lastModified.toSecsSinceEpoch() > syncTime) {
        qDebug() << "checking merge conflict";
        if (file.open(QIODevice::ReadOnly)) {
            // check if it's just the same file
            if (file.readAll() == remoteData) {
                qDebug() << "same file";
                mSettings.setValue(relativePath,
                    qMax(info.lastModified().toSecsSinceEpoch(), lastModified.toSecsSinceEpoch()));
                return;
            }
            file.close();
        }
    }

    if (info.lastModified() < lastModified) {
        if (file.open(QIODevice::WriteOnly)) {
            file.write(remoteData);
            qDebug() << "setting file time" << localPath << lastModified << info.lastModified();
            if (!file.setFileTime(lastModified, QFileDevice::FileModificationTime)) {
                qDebug() << "failed to update time";
            }
            file.close();
            mSettings.setValue(relativePath, lastModified.toSecsSinceEpoch());
            emit fileUpdated(localPath);
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
