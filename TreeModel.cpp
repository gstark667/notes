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
    , rootItem(std::make_shared<TreeItem>("", "")) {}


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
    } else if (role == Qt::UserRole) {
        return item->path();
    } else if (role == Qt::UserRole + 1) {
        return item->childCount() > 0;
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
    rootItem = std::make_shared<TreeItem>("", mPath);
    setupModelData(mPath, rootItem.get());
    endResetModel();
}


void TreeModel::setupModelData(QString path, TreeItem *parent)
{
    QFileInfo info(path);

    if (info.isHidden()) {
        return;
    }

    auto newItem = new TreeItem(info.fileName(), path, parent);

    if (info.isDir()) {
        QDirIterator it(path);
        while (it.hasNext()) {
            setupModelData(it.next(), newItem);
        }
    }

    parent->appendChild(newItem);
}
