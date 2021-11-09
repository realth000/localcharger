#ifndef WEBSENDER_H
#define WEBSENDER_H

#include <QWebSocket>
#include <QObject>

class WebSender : public QWebSocket
{
    Q_OBJECT
        public:
                 explicit WebSender(QObject *parent = nullptr);
};

#endif // WEBSENDER_H
