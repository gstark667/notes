#include "FileModel.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

FileModel::FileModel (QObject* parent) : QObject(parent) {}


Q_INVOKABLE QString FileModel::open(QString path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Failed to open file: %s", qUtf8Printable(path));
        return QString();
    }

    QTextStream stream(&file);
    QString contents = stream.readAll();
    file.close();

    mCurrentPath = path;

    return contents;
}


Q_INVOKABLE bool FileModel::save(QString data) {
    QFile file(mCurrentPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Failed to open file: %s", qUtf8Printable(mCurrentPath));
        return false;
    }

    QTextStream out(&file);
    out << data;

    return true;
}