#ifndef H_HIGHLIGHTER
#define H_HIGHLIGHTER

#include <QSyntaxHighlighter>
#include <QQmlEngine>
#include <QQuickTextDocument>
#include <QRegularExpression>

class Highlighter;

class Highlighter: public QSyntaxHighlighter {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)

public:

    explicit Highlighter(QObject *parent = nullptr);

signals:
    void textDocumentChanged();
    void highlightBlock(const QVariant& text);

protected:
    QQuickTextDocument* textDocument() const { return mTextDocument; };
    void setTextDocument(QQuickTextDocument* textDocument);

    virtual void highlightBlock(const QString &text);

private:
    QQuickTextDocument* mTextDocument;
    QList<QPair<QRegularExpression, QTextCharFormat>> mFormats;
};

#endif
