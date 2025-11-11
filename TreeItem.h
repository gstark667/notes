#ifndef TREE_ITEM_H
#define TREE_ITEM_H

#include <QVariant>
#include <QList>
#include <memory>

class TreeItem
{
public:
    explicit TreeItem(QString data, QString path, TreeItem *parentItem = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    size_t childCount() const { return mChildItems.size(); };
    QString data() const { return mData; };
    QString path() const { return mPath; };
    int row() const;
    TreeItem *parentItem() { return mParentItem; };

private:
    std::vector<TreeItem*> mChildItems;
    QString mData;
    QString mPath;
    TreeItem *mParentItem;
};

#endif // TREE_ITEM_H
