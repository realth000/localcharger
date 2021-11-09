#ifndef MAINUI_H
#define MAINUI_H

#include <QtWidgets/QWidget>
#include "src/core/websender.h"
#include "src/core/webrecver.h"

QT_BEGIN_NAMESPACE
    namespace Ui { class MainUi; }
QT_END_NAMESPACE

class MainUi : public QWidget
{
Q_OBJECT

public:
    explicit MainUi(QWidget *parent = nullptr);
    void initUi();
    ~MainUi();

private slots:
    void on_sendMsgButton_clicked();
    void recoredRecvedMsg(const QString &msg);

private:
    Ui::MainUi *ui;
    WebSender sender;
    WebRecver recver;
};
#endif // MAINUI_H
