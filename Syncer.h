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
    Q_INVOKABLE void sync();

    void setPath(QString path) {
        mLocalRoot = path;
        mHistoryRoot = QDir(mLocalRoot.filePath(".history"));
    };

    QString toLocalPath(QString remotePath);
    QString historyPath(QString path);

signals:
    void fileCreated(QString path);
    void fileUpdated(QString path);

public slots:
    void listFinished();
    //void getFile(QString path);
    void putFile(QString path);
    void putDir(QString startPath, QSet<QString> ignore = {});
    void makeHistory(QString path);
    QByteArray getHistory(QString path);
    void itemRead();
    void itemWritten();
    void dirCreated();
    void error(QString message);

private:
    QWebdav mWebdav;
    QWebdavDirParser mParser;

    QSettings mSettings;

    QString mHost;
    QString mUrl;
    QString mUsername;
    QString mPassword;

    QString mRootPath;
    QDir mLocalRoot;
    QDir mHistoryRoot;
};

#endif
