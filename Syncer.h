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

public:
    explicit Syncer(QObject *parent = nullptr);

    Q_INVOKABLE void open(QString url, QString username, QString password);

public slots:
    void finished();
    void itemRead();
    void error(QString message);

private:
    QWebdav mWebdav;
    QWebdavDirParser mParser;

    QString mHost;
    QString mUrl;
    QString mUsername;
    QString mPassword;

    QString mRootPath;
};

#endif
