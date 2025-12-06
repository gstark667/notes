#include <QList>
#include <QByteArray>

#include <qlogging.h>

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

DiffState findPath(QList<QList<bool>> grid, QList<QByteArray> removes, QList<QByteArray> adds) {
    QList<DiffState> states;
    states.push_back({{}, 0, 0});
    while (true) {
        QList<DiffState> newStates;
        for (auto state: states) {
            while (state.x < removes.size() && state.y < adds.size() && grid[state.y][state.x]) {
                state.changes.push_back({DiffAction::NO_CHANGE, removes[state.x]});
                state.x++; state.y++;
            }
            // go right
            if (state.x < removes.size()) {
                DiffState remove = state;
                remove.changes.push_back({DiffAction::REMOVE, removes[remove.x]});
                remove.x++;
                if (remove.x == removes.size() && remove.y == adds.size()) {
                    return remove;
                }
                newStates.push_back(remove);
            }
            // go down
            if (state.y < adds.size()) {
                DiffState add = state;
                add.changes.push_back({DiffAction::ADD, adds[add.y]});
                add.y++;
                if (add.x == removes.size() && add.y == adds.size()) {
                    return add;
                }
                newStates.push_back(add);
            }
        }
        states = newStates;
    }
}

void diff(QByteArray local, QByteArray remote) {
    auto removes = remote.split('\n');
    auto adds    = local.split('\n');

    // build the grid
    QList<QList<bool>> grid;
    for (size_t y = 0; y < adds.size(); y++) {
        QList<bool> row;
        for (size_t x = 0; x < removes.size(); x++) {
            row.append(adds[y] == removes[x]);
        }
        grid.append(row);
    }

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
}
