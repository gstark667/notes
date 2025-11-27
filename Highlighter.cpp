#include "Highlighter.h"

QList<QPair<QRegularExpression, QTextCharFormat>> makeFormats() {
    QList<QPair<QRegularExpression, QTextCharFormat>> formats;

    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);
    QRegularExpression boldExpression("\\*\\*[^\\*]+\\*\\*");
    formats.push_back({boldExpression, boldFormat});

    QTextCharFormat italicFormat;
    italicFormat.setFontItalic(true);
    QRegularExpression italicExpression("_[^_]+_");
    formats.push_back({italicExpression, italicFormat});

    QTextCharFormat monospaceFormat;
    monospaceFormat.setFontFamilies({"Monospace"});
    QRegularExpression monospaceExpression("\\`[^\\`]+\\`");
    formats.push_back({monospaceExpression, monospaceFormat});

    QTextCharFormat h3Format;
    h3Format.setFontPointSize(16);
    h3Format.setFontWeight(QFont::Bold);
    QRegularExpression h3Expression("^###.+$");
    formats.push_back({h3Expression, h3Format});

    QTextCharFormat h2Format;
    h2Format.setFontPointSize(20);
    h2Format.setFontWeight(QFont::Bold);
    QRegularExpression h2Expression("^##.+$");
    formats.push_back({h2Expression, h2Format});

    QTextCharFormat h1Format;
    h1Format.setFontPointSize(24);
    h1Format.setFontWeight(QFont::Bold);
    QRegularExpression h1Expression("^#.+$");
    formats.push_back({h1Expression, h1Format});

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
    int position = 0;
    while (position < text.length()) {
        bool matched = false;
        for (auto it: mFormats) {
            auto match = it.first.match(text, position);
            if (match.hasMatch() && match.capturedStart() == position) {
                setFormat(match.capturedStart(), match.capturedLength(), it.second);
                position += match.capturedLength();
                matched = true;
                break;
            }
        }
        if (!matched) {
            position++;
        }
    }
}
