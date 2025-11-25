#include "TreeItem.h"


TreeItem::TreeItem(QString data, QString path, bool isDirectory, TreeItem *parent)
    : mData(data), mPath(path), mIsDirectory(isDirectory), mParentItem(parent) {}


TreeItem::~TreeItem() {
    for (auto child: mChildItems) {
        delete child;
    }
}


void TreeItem::insertChild(TreeItem *child, size_t row)
{
    mChildItems.insert(mChildItems.begin() + row, child);
}


void TreeItem::appendChild(TreeItem *child)
{
    mChildItems.push_back(child);
}


void TreeItem::removeChild(size_t row) {
    mChildItems.erase(mChildItems.begin() + row);
}


TreeItem *TreeItem::child(int row)
{
    return row >= 0 && row < childCount() ? mChildItems.at(row) : nullptr;
}


TreeItem *TreeItem::find(QString data)
{
    for (auto item: mChildItems) {
        if (item->data() == data) {
            return item;
        }
    }
    return nullptr;
}


int TreeItem::row() const
{
    if (mParentItem == nullptr)
        return 0;
    const auto it = std::find_if(mParentItem->mChildItems.cbegin(), mParentItem->mChildItems.cend(),
                                 [this](const TreeItem *treeItem) {
                                     return treeItem == this;
                                 });

    if (it != mParentItem->mChildItems.cend())
        return std::distance(mParentItem->mChildItems.cbegin(), it);
    Q_ASSERT(false); // should not happen
    return -1;
}
