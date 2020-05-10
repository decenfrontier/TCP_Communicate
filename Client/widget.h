#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>

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
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();

private slots:
    void on_btnSend_clicked();

    void on_btnImage_clicked();

private:
    Ui::Widget *ui;
    QTcpSocket tcpSocket;
    int imageIndex;
    quint32 LastPacksize;
};

#endif // WIDGET_H
