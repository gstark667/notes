#ifndef FILE_MODEL_H
#define FILE_MODEL_H

#include <QObject>
#include <QString>


class FileModel : public QObject{
   Q_OBJECT
public:
    explicit FileModel (QObject* parent = 0);
    Q_INVOKABLE QString open(QString path);
    Q_INVOKABLE bool    save(QString data);
    Q_INVOKABLE bool    createFolder(QString path, QString name);
    Q_INVOKABLE bool    createFile(QString path, QString name);
    Q_INVOKABLE bool    remove(QString path);

private:
    QString mCurrentPath;
};

#endif // FILE_MODEL_H
