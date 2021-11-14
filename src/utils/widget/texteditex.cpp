#include "texteditex.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QMimeData>
#include <QtCore/QtMath>

TextEditEx::TextEditEx(QWidget *parent) : QTextEdit(parent)
{

}

void TextEditEx::append(const QString &text)
{
    splitInput(QStringLiteral("parentAppend"), text);
}

void TextEditEx::insertHtml(const QString &text)
{
    splitInput(QStringLiteral("parentInsertHtml"), text);
}

void TextEditEx::insertPlainText(const QString &text)
{
    splitInput(QStringLiteral("parentInsertPlainText"), text);
}

void TextEditEx::insertFromMimeData(const QMimeData *source)
{
    splitInput(QStringLiteral("parentInsertFromMimeData"), source->text());
}

void TextEditEx::parentAppend(const QString &text)
{
    QTextEdit::append(text);
}

void TextEditEx::parentInsertFromMimeData(const QString &sourceString)
{
    QMimeData *d = new QMimeData;
    d->setText(sourceString);
    QTextEdit::insertFromMimeData(d);
}

void TextEditEx::parentInsertHtml(const QString &text)
{
    QTextEdit::insertHtml(text);
}

void TextEditEx::parentInsertPlainText(const QString &text)
{
    QTextEdit::insertPlainText(text);
}

void TextEditEx::splitInput(const QString &invokeMethodName, const QString &input)
{
    const int maxTimes = qFloor(qreal(input.length() / APPEND_LENGTH)) + 1;
    for(int times = 0; times <= maxTimes; times++){
        QCoreApplication::processEvents();
        QMetaObject::invokeMethod(this, invokeMethodName.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(QString, input.mid(APPEND_LENGTH * times, APPEND_LENGTH)));
    }
}
