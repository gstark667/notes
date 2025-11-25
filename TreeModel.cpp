#include "TreeModel.h"
#include "TreeItem.h"
#include <QFile>
#include <QStringList>
#include <QStack>
#include <QDirIterator>

#include <iostream>


using namespace Qt::StringLiterals;


TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , rootItem(std::make_shared<TreeItem>("", "", true)) {}


TreeModel::~TreeModel() = default;


int TreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}


QHash<int, QByteArray> TreeModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Qt::UserRole]    = "path";
    roles[Qt::UserRole+1]  = "isDirectory";
    return roles;
}


QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto *item = static_cast<const TreeItem*>(index.internalPointer());

    if (role == Qt::DisplayRole) {
        return item->data();
    } else if (role == Qt::UserRole) { // path
        return item->path();
    } else if (role == Qt::UserRole + 1) { // isDirectory
        return item->isDirectory();
    }
    return {};
}


Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    return index.isValid()
               ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);
}


QVariant TreeModel::headerData(int, Qt::Orientation orientation,
                               int role) const
{
    return orientation == Qt::Horizontal && role == Qt::DisplayRole
               ? rootItem->data() : QVariant{};
}


QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    TreeItem *parentItem = parent.isValid()
                               ? static_cast<TreeItem*>(parent.internalPointer())
                               : rootItem.get();

    if (auto *childItem = parentItem->child(row))
        return createIndex(row, column, childItem);
    return {};
}


QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    auto *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    return parentItem != rootItem.get()
               ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex{};
}


int TreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    const TreeItem *parentItem = parent.isValid()
        ? static_cast<const TreeItem*>(parent.internalPointer())
        : rootItem.get();

    return parentItem->childCount();
}


void TreeModel::setPath(QString path) {
    QUrl url(path);
    if (url.isValid() && url.isLocalFile()) {
        mPath = url.toLocalFile();
    } else {
        mPath = path;
    }

    beginResetModel();
    rootItem = std::make_shared<TreeItem>("", mPath, true);
    setupModelData(mPath, rootItem.get());
    endResetModel();
}


TreeItem* TreeModel::find(const QString path) const {
    QDir dir(mPath);
    dir.cdUp();
    QString relative = dir.relativeFilePath(path);

    QList<QString> segments;

    while (true) {
        QFileInfo file(relative);
        qDebug() << file.fileName() << " " << file.dir();

        segments.push_front(file.fileName());

        if (file.dir().path() == QFileInfo(file.dir().path()).dir().path()) {
            break;
        }
        relative = file.dir().path();
    }

    TreeItem *curr = rootItem.get();
    for (auto segment: segments) {
        curr = curr->find(segment);
    }

    return curr;
}


bool TreeModel::create(QString path, QString name, bool isDirectory) {
    // insert a new file by name
    //TreeItem *parentItem = rootItem->child(0);

    QDir dir(path);
    QString filePath = dir.filePath(name);

    TreeItem *parentItem = find(path);
    qDebug() << "create " << filePath << " " << isDirectory << parentItem->path();

    bool created = false;
    
    if (isDirectory) {
        created = dir.mkdir(name);
    } else {
        QFile file(filePath);
        created = file.open(QIODevice::WriteOnly | QIODevice::Text);
    }

    if (created) {
        beginInsertRows(createIndex(parentItem->row(), 0, parentItem), 0, 0);
        parentItem->insertChild(
            new TreeItem(name, filePath, isDirectory, parentItem),
            0);
        endInsertRows();
        return true;
    } else {
        qWarning("Failed to create file: %s", qUtf8Printable(filePath));
    }

    return false;
}



bool TreeModel::remove(QString path) {

    TreeItem *item = find(path);
    TreeItem *parentItem = item->parentItem();
    int row = item->row();

    bool removed = false;

    QFileInfo info(path);
    if (info.isDir()) {
        qDebug() << "remove dir: " << path;
        removed = QDir(path).removeRecursively();
    } else {
        qDebug() << "remove file: " << path;
        removed = QFile(path).remove();
    }

    if (removed) {
        beginRemoveRows(createIndex(parentItem->row(), 0, parentItem), row, row);
        parentItem->removeChild(row);
        delete item;
        endRemoveRows();
        return true;
    } else {
        return false;
    }
}


QString TreeModel::open(QString path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Failed to open file: %s", qUtf8Printable(path));
        return QString();
    }

    QTextStream stream(&file);
    QString contents = stream.readAll();
    file.close();

    return contents;
}


bool TreeModel::save(QString path, QString data) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Failed to open file: %s", qUtf8Printable(path));
        return false;
    }

    QTextStream out(&file);
    out << data;

    return true;
}


void TreeModel::setupModelData(QString path, TreeItem *parent)
{
    QFileInfo info(path);

    if (info.isHidden()) {
        return;
    }

    auto newItem = new TreeItem(info.fileName(), path, info.isDir(), parent);

    if (info.isDir()) {
        QDirIterator it(path);
        while (it.hasNext()) {
            setupModelData(it.next(), newItem);
        }
    }

    parent->appendChild(newItem);
}
