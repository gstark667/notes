#include <QList>
#include <QPair>
#include <QSet>
#include <QByteArray>

#include <qlogging.h>

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
    for (size_t i = 0; i < removes.size() - 1; ++i) {
        removes[i].append('\n');
    }
    auto adds    = newData.split('\n');
    for (size_t i = 0; i < adds.size() - 1; ++i) {
        adds[i].append('\n');
    }

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

void diff3(QByteArray common, QByteArray local, QByteArray remote) {
    qDebug() << "==== Local Changes ===";
    auto localDiff = diff(common, local);
    qDebug() << "=== Remote Changes ===";
    auto remoteDiff = diff(common, remote);

    auto &lChanges = localDiff.changes;
    auto &rChanges = remoteDiff.changes;

    QString left;
    QString right;

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
                left += lChange.line;
            }
        } else {
            if (left != "" && right != "") {
                qDebug() << "<<<<<<<<";
                qDebug() << left;
                qDebug() << "========";
                qDebug() << right;
                qDebug() << ">>>>>>>>";
                //qDebug() << "conflict" << left << right;
            } else if (left != "") {
                qDebug() /*<< "using left"*/ << left;
            } else if (right != "") {
                qDebug() /*<< "using right"*/ << right;
            }
            left = "";
            right = "";

            //qDebug() << "same" << actionString(lChange.action) << lChange.line;
            if (lChange.action != DiffAction::REMOVE) {
                qDebug() << lChange.line;
            }
        }
    }

    /*for (auto lChange: localDiff.changes) {
        for (auto rChange: remoteDiff.changes) {
            if (lChange.line == rChange.line && lChange.action == rChange.action) {
                qDebug() << "same" << lChange.line;
            }
        }
        }*/
}
