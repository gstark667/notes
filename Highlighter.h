#ifndef H_HIGHLIGHTER
#define H_HIGHLIGHTER

#include <QSyntaxHighlighter>
#include <QQmlEngine>

class Highlighter;

class Highlighter: public QSyntaxHighlighter {
    Q_OBJECT
    QML_NAMED_ELEMENT(Highlighter)
    Q_DISABLE_COPY_MOVE(Highlighter)

public:

    explicit Highlighter(QObject *parent = nullptr);

    virtual void highlightBlock(const QString &text) {};
};

#endif