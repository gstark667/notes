#include "TreeItem.h"


TreeItem::TreeItem(QString data, QString path, TreeItem *parent)
    : mData(data), mPath(path), mParentItem(parent) {}


TreeItem::~TreeItem() {
    for (auto child: mChildItems) {
        delete child;
    }
}


void TreeItem::appendChild(TreeItem *child)
{
    mChildItems.push_back(child);
}


TreeItem *TreeItem::child(int row)
{
    return row >= 0 && row < childCount() ? mChildItems.at(row) : nullptr;
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
