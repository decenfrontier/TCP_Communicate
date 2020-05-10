#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QList>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void onNewConnection();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();



private slots:
    void on_btnSend_clicked();

    void on_btnImage_clicked();

private:
    Ui::Widget *ui;
    QTcpServer server;
    QList<QTcpSocket*> clients;
    int imageIndex;
    quint32 LastPacksize;   // 数据包大小(含包头)
};

#endif // WIDGET_H
