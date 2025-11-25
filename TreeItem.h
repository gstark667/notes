#ifndef TREE_ITEM_H
#define TREE_ITEM_H

#include <QVariant>
#include <QList>
#include <memory>

class TreeItem
{
public:
    explicit TreeItem(QString data, QString path, bool isDirectory, TreeItem *parentItem = nullptr);
    ~TreeItem();

    void insertChild(TreeItem *child, size_t row);
    void appendChild(TreeItem *child);
    void removeChild(size_t row);

    TreeItem *child(int row);
    TreeItem *find(QString data);
    size_t childCount() const { return mChildItems.size(); };
    QString data() const { return mData; };
    QString path() const { return mPath; };
    bool isDirectory() const { return mIsDirectory; };
    int row() const;
    TreeItem *parentItem() { return mParentItem; };

private:
    std::vector<TreeItem*> mChildItems;
    QString mData;
    QString mPath;
    bool mIsDirectory;
    TreeItem *mParentItem;
};

#endif // TREE_ITEM_H
