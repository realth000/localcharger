#ifndef TEXTEDITEX_H
#define TEXTEDITEX_H

#include <QTextEdit>

#define APPEND_LENGTH 3000

class TextEditEx : public QTextEdit
{
    Q_OBJECT
public:
    TextEditEx(QWidget *parent = nullptr);

public slots:
    void append(const QString &text);
    void insertHtml(const QString &text);
    void insertPlainText(const QString &text);

protected:
    void insertFromMimeData(const QMimeData *source) override;

private slots:
    void parentAppend(const QString &text);
    void parentInsertFromMimeData(const QString &sourceString);
    void parentInsertHtml(const QString &text);
    void parentInsertPlainText(const QString &text);

private:
    void splitInput(const QString &invokeMethodName, const QString &input);
};

#endif // TEXTEDITEX_H
