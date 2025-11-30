#ifndef H_SYNCER
#define H_SYNCER

#include <QQmlEngine>

#include <qtmetamacros.h>
#include <qwebdav.h>
#include <qwebdavdirparser.h>
#include <qwebdavitem.h>

class Syncer: public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString path WRITE setPath)

public:
    explicit Syncer(QObject *parent = nullptr);

    Q_INVOKABLE void open(QString url, QString username, QString password);

    void setPath(QString path) { mLocalRoot = path; };

    QString toLocalPath(QString remotePath);

public slots:
    void finished();
    void itemRead();
    void itemWritten();
    void error(QString message);

private:
    QWebdav mWebdav;
    QWebdavDirParser mParser;

    QString mHost;
    QString mUrl;
    QString mUsername;
    QString mPassword;

    QString mRootPath;
    QString mLocalRoot;
};

#endif
