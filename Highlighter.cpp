#include "Highlighter.h"

QList<QPair<QRegularExpression, QTextCharFormat>> makeFormats() {
    QList<QPair<QRegularExpression, QTextCharFormat>> formats;

    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);
    QRegularExpression boldExpression("\\*\\*[A-Za-z]+\\*\\*");
    formats.push_back({boldExpression, boldFormat});

    QTextCharFormat italicFormat;
    italicFormat.setFontItalic(true);
    QRegularExpression italicExpression("_[A-Za-z]+_");
    formats.push_back({italicExpression, italicFormat});

    QTextCharFormat monospaceFormat;
    monospaceFormat.setFontFamilies({"Monospace"});
    QRegularExpression monospaceExpression("\\`[\\sA-Za-z]+\\`");
    formats.push_back({monospaceExpression, monospaceFormat});

    QTextCharFormat h1Format;
    h1Format.setFontPointSize(24);
    h1Format.setFontWeight(QFont::Bold);
    QRegularExpression h1Expression("^#[\\sA-Za-z]+$");
    formats.push_back({h1Expression, h1Format});

    QTextCharFormat h2Format;
    h2Format.setFontPointSize(20);
    h2Format.setFontWeight(QFont::Bold);
    QRegularExpression h2Expression("^##[\\sA-Za-z]+$");
    formats.push_back({h2Expression, h2Format});

    QTextCharFormat h3Format;
    h3Format.setFontPointSize(16);
    h3Format.setFontWeight(QFont::Bold);
    QRegularExpression h3Expression("^###[\\sA-Za-z]+$");
    formats.push_back({h3Expression, h3Format});

    return formats;
}

Highlighter::Highlighter(QObject *parent): QSyntaxHighlighter(parent) {
    mFormats = makeFormats();
}

void Highlighter::setTextDocument(QQuickTextDocument* textDocument) {
    if (textDocument == mTextDocument)
    {
        return;
    }

    mTextDocument = textDocument;

    QTextDocument* doc = mTextDocument->textDocument();
    setDocument(doc);

    emit textDocumentChanged();
}

void Highlighter::highlightBlock(const QString &text) {
    for (auto it: mFormats) {
        auto i = it.first.globalMatch(text);
        while (i.hasNext()) {
            auto match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), it.second);
        }
    }
}