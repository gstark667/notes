#ifndef H_DIFFMODEL
#define H_DIFFMODEL

#include <QList>
#include <QPair>
#include <QSet>
#include <QByteArray>

#include <QAbstractListModel>
#include <QQmlEngine>

#include <qlogging.h>

#include <qtmetamacros.h>
#include <string>
#include <iostream>

enum DiffAction {
    NO_CHANGE,
    ADD,
    REMOVE
};

struct DiffChange {
    DiffAction action;
    QByteArray line;
};

struct DiffState {
    QList<DiffChange> changes;
    size_t x;
    size_t y;
};

struct MergeItem {
    bool conflicting;
    QByteArray data;
    QByteArray left;
    QByteArray right;
};

class DiffModel : public QAbstractListModel
{
Q_OBJECT
QML_ELEMENT

Q_PROPERTY(bool conflicting READ isConflicting WRITE setConflicting NOTIFY conflictingChanged)

public:
    DiffModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setDiff(QList<MergeItem> aDiff);
    void setConflicting(bool aConflicting) { mConflicting = aConflicting; qDebug() << "set conflicting" << aConflicting; };
    bool isConflicting() { return mConflicting; };

    Q_INVOKABLE QByteArray getData();

public slots:
    void createDiff(QByteArray common, QByteArray local, QByteArray remote);
    void mergeLeft(int index);
    void mergeRight(int index);
    void update(int index, QByteArray text);
    void useLocal();
    void useRemote();

signals:
    void diffCreated();
    void conflictingChanged();

private:
    QList<MergeItem> mDiff;

    QByteArray mCommon;
    QByteArray mLocal;
    QByteArray mRemote;
    bool mConflicting;

    void checkStatus();
};

QString actionString(DiffAction action);

DiffState findPath(QList<QList<bool>> grid, QList<QByteArray> removes, QList<QByteArray> adds);

DiffState diff(QByteArray oldData, QByteArray newData);

QList<MergeItem> diff3(QByteArray common, QByteArray local, QByteArray remote);

#endif
