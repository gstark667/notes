#include "Syncer.h"
#include "qwebdav.h"
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
    mParser.listDirectory(&mWebdav, "/", true);
}

void Syncer::finished() {
    qDebug() << "Syncer: connected";

    QList<QWebdavItem> list = mParser.getList();
    QWebdavItem item;
    foreach(item, list) {
        qDebug() << item.name() << " " << item.path();
        QNetworkReply *reply = mWebdav.get(item.path());
        connect(reply, SIGNAL(readyRead()), this, SLOT(itemRead()));
    }
}

void Syncer::itemRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (reply==0)
        return;

    QByteArray ba = reply->readAll();
    qDebug() << reply->url().toString(QUrl::RemoveUserInfo) << ":" <<
        // example get path in dir
        QDir(mRootPath).relativeFilePath(reply->url().path()) << ba;
    // example get modification time
    qDebug() << reply->headers().value("last-modified");
}

void Syncer::error(QString message) {
    qWarning() << "Syncer: " << message;
}
