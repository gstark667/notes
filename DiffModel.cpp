#include "DiffModel.h"
#include <qnamespace.h>

#include <iostream>

DiffModel::DiffModel(QObject *parent):
    QAbstractListModel(parent) {
}

QHash<int, QByteArray> DiffModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "conflicting";
    roles[Qt::DisplayRole+1] = "display";
    roles[Qt::DisplayRole+2] = "leftContent";
    roles[Qt::DisplayRole+3] = "rightContent";
    return roles;
}

int DiffModel::rowCount(const QModelIndex &parent) const {
    return mDiff.size();
}

QVariant DiffModel::data(const QModelIndex &index, int role) const {
    auto &item = mDiff[index.row()];
    if (role == Qt::DisplayRole) {
        return item.conflicting;
    } else if (role == Qt::DisplayRole+1) {
        return item.data;
    } else if (role == Qt::DisplayRole+2) {
        return item.left;
    } else if (role == Qt::DisplayRole+3) {
        return item.right;
    }
    return "";
}

void DiffModel::setDiff(QList<MergeItem> aDiff) {
    beginResetModel();
    mDiff = aDiff;
    endResetModel();

    emit diffCreated();

    checkStatus();
}

void DiffModel::setConflicting(bool aConflicting) {
    bool doEmit = aConflicting != mConflicting;
    mConflicting = aConflicting;
    if (doEmit) {
        emit conflictingChanged();
    }
}

void DiffModel::createDiff(QByteArray common, QByteArray local, QByteArray remote) {
    mCommon = common;
    mLocal = local;
    mRemote = remote;

    setDiff(diff3(common, local, remote));
}

void DiffModel::mergeLeft(int index) {
    auto &diff = mDiff[index];
    qDebug() << "use left" << diff.left;
    diff.data = diff.left;
    diff.conflicting = false;
    dataChanged(createIndex(index, 0), createIndex(index, 0));
    checkStatus();
}

void DiffModel::mergeRight(int index) {
    auto &diff = mDiff[index];
    qDebug() << "use right" << diff.right;
    diff.data = diff.right;
    diff.conflicting = false;
    dataChanged(createIndex(index, 0), createIndex(index, 0));
    checkStatus();
}

void DiffModel::update(int index, QByteArray text) {
    auto &diff = mDiff[index];
    diff.data = text;
    dataChanged(createIndex(index, 0), createIndex(index, 0));
}

void DiffModel::useLocal() {
    beginResetModel();
    mDiff = {{false, mLocal, "", ""}};
    endResetModel();
    checkStatus();
}

void DiffModel::useRemote() {
    beginResetModel();
    mDiff = {{false, mRemote, "", ""}};
    endResetModel();
    checkStatus();
}

void DiffModel::checkStatus() {
    bool isResolved = true;
    for (auto &conflict: mDiff) {
        isResolved &= !conflict.conflicting;
    }
    setConflicting(!isResolved);
}

QByteArray DiffModel::getData() {
    QByteArray output;
    for (size_t i = 0; i < mDiff.size(); ++i) {
        if (i > 0) {
            output += "\n";
        }
        auto &diff = mDiff[i];
        if (diff.data != "") {
            output += diff.data;
        } else if (diff.left != "") {
            output += diff.left;
        } else {
            output += diff.right;
        }
    }
    return output;
}

QString actionString(DiffAction action) {
    switch (action) {
    case DiffAction::ADD:
        return "+";
    case DiffAction::REMOVE:
        return "-";
    default:
        return " ";
    }
}

DiffState findPath(QList<QList<bool>> grid, QList<QByteArray> removes, QList<QByteArray> adds) {
    QList<DiffState> states;
    QSet<QPair<size_t, size_t>> locations;
    states.push_back({{}, 0, 0});
    locations.insert({0, 0});
    while (true) {
        QList<DiffState> newStates;
        for (auto state: states) {
            while (state.x < removes.size() && state.y < adds.size() && grid[state.y][state.x]) {
                state.changes.push_back({DiffAction::NO_CHANGE, removes[state.x]});
                state.x++; state.y++;
                locations.insert({state.x, state.y});
            }
            if (state.x == removes.size() && state.y == adds.size()) {
                return state;
            }
            // go right
            if (state.x < removes.size()) {
                DiffState remove = state;
                remove.changes.push_back({DiffAction::REMOVE, removes[remove.x]});
                remove.x++;
                if (remove.x == removes.size() && remove.y == adds.size()) {
                    return remove;
                }
                if (!locations.contains({remove.x, remove.y})) {
                    locations.insert({remove.x, remove.y});
                    newStates.push_back(remove);
                }
            }
            // go down
            if (state.y < adds.size()) {
                DiffState add = state;
                add.changes.push_back({DiffAction::ADD, adds[add.y]});
                add.y++;
                if (add.x == removes.size() && add.y == adds.size()) {
                    return add;
                }
                if (!locations.contains({add.x, add.y})) {
                    locations.insert({add.x, add.y});
                    newStates.push_back(add);
                }
            }
        }
        states = newStates;
    }
}

DiffState diff(QByteArray oldData, QByteArray newData) {
    auto removes = oldData.split('\n');
    auto adds    = newData.split('\n');

    // build the grid
    QList<QList<bool>> grid;
    for (size_t y = 0; y < adds.size(); y++) {
        QList<bool> row;
        for (size_t x = 0; x < removes.size(); x++) {
            row.append(adds[y] == removes[x]);
        }
        grid.append(row);
    }

    std::string line = "    \t";
    for (size_t x = 0; x < removes.size(); x++) {
        line += " " + removes[x].toStdString().substr(0, 1);
    }
    std::cout << line << std::endl;
    for (size_t y = 0; y <= adds.size(); y++) {
        line = "    \t";
        for (size_t x = 0; x < removes.size(); x++) {
            line += "*-";
            if (x == removes.size() - 1) {
                line += "*";
            }
        }
        std::cout << line << " " << y << std::endl;
        if (y < adds.size()) {
            line = "";
            for (size_t x = 0; x < removes.size(); x++) {
                line += "|";
                line += grid[y][x] ? '\\' : ' ';
                if (x == removes.size() - 1) {
                    line += "|";
                }
            }
            std::cout << adds[y].toStdString().substr(0, 4) << "\t" << line << std::endl;
        }
    }
    line = "    \t";
    for (size_t x = 0; x < removes.size(); x++) {
        line += std::to_string(x) + " ";
    }
    std::cout << line << removes.size() << std::endl;

    auto result = findPath(grid, removes, adds);
    for (auto change: result.changes) {
        QString pre = " ";
        if (change.action == DiffAction::ADD) {
            pre = "+";
        } else if (change.action == DiffAction::REMOVE) {
            pre = "-";
        }
        qDebug() << pre + " " + change.line;
    }
    return result;
}

QList<MergeItem> diff3(QByteArray common, QByteArray local, QByteArray remote) {
    qDebug() << "==== Local Changes ===";
    auto localDiff = diff(common, local);
    qDebug() << "=== Remote Changes ===";
    auto remoteDiff = diff(common, remote);

    auto &lChanges = localDiff.changes;
    auto &rChanges = remoteDiff.changes;

    bool leftStarted = false;
    QByteArray left;
    bool rightStarted = false;
    QByteArray right;
    bool centerStarted = false;
    QByteArray center;

    QList<MergeItem> output;

    size_t rPos = 0;
    for (size_t l = 0; l < lChanges.size(); ++l) {
        bool match = false;
        auto &lChange = lChanges[l];
        for (size_t r = rPos; r < rChanges.size(); ++r) {
            auto &rChange = rChanges[r];
            if (lChange.line == rChange.line && lChange.action == rChange.action) {
                for (size_t tmp = rPos; tmp < r; ++tmp) {
                    //qDebug() << "right" << actionString(rChanges[tmp].action) << rChanges[tmp].line;
                    if (rChanges[tmp].action != DiffAction::REMOVE) {
                        if (rightStarted) {
                            right += "\n";
                        }
                        rightStarted = true;
                        right += rChanges[tmp].line;
                    }
                }
                match = true;
                rPos = r + 1;
                break;
            }
        }
        if (!match) {
            //qDebug() << "left" << actionString(lChange.action) << lChange.line;
            if (lChange.action != DiffAction::REMOVE) {
                if (leftStarted) {
                    left += "\n";
                }
                leftStarted = true;
                left += lChange.line;
            }

            if (centerStarted) {
                output.push_back({false, center});
                center = "";
                centerStarted = false;
            }
        } else {
            if (left != "" && right != "") {
                qDebug() << "<<<<<<<<";
                qDebug() << left;
                qDebug() << "========";
                qDebug() << right;
                qDebug() << ">>>>>>>>";
                output.push_back({true, "", left, right});
                //qDebug() << "conflict" << left << right;
            } else if (left != "") {
                qDebug() /*<< "using left"*/ << left;
                output.push_back({false, left});
            } else if (right != "") {
                qDebug() /*<< "using right"*/ << right;
                output.push_back({false, right});
            }
            left = "";
            leftStarted = false;
            right = "";
            rightStarted = false;

            //qDebug() << "same" << actionString(lChange.action) << lChange.line;
            if (lChange.action != DiffAction::REMOVE) {
                qDebug() << lChange.line;
                if (centerStarted) {
                    center += "\n";
                }
                centerStarted = true;
                center += lChange.line;
            }
        }
    }
    if (centerStarted) {
        output.push_back({false, center});
    }
    return output;
}
