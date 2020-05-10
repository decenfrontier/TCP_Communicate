#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QDateTime>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0,2);    // 上面占2份
    ui->splitter->setStretchFactor(1,1);    // 下面占1份

    connect(&tcpSocket,SIGNAL(connected()),
            this,SLOT(onConnected()));
    connect(&tcpSocket,SIGNAL(disconnected()),
            this,SLOT(onDisconnected()));
    connect(&tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(onError(QAbstractSocket::SocketError)));
    connect(&tcpSocket,SIGNAL(readyRead()),
            this,SLOT(onReadyRead()));

    // 客户端主动连接服务端
    tcpSocket.connectToHost("127.0.0.1",5485);  // host地址为本机,端口号为5485

    imageIndex = 0;
    LastPacksize = 0;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onConnected()
{
    qDebug() << "connected";
}

void Widget::onDisconnected()
{
    qDebug() << "disconnected";

    QObject* obj = this->sender();
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(obj);
    if(socket == 0) return; // 说明是客户端先关闭,不用做其他操作
    socket->close();    // 如果是服务端先关闭,就关闭这个socket

}

void Widget::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "error: " << socketError;
}

void Widget::onReadyRead()
{
    QObject* obj = this->sender();
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(obj);

    quint64 sizeNow = 0;
    QDataStream stream(socket);
    do
    {
        // 当前缓冲区里的数据大小
        sizeNow = socket->bytesAvailable(); // 获取等待读取的传入字节数。

        if(LastPacksize == 0)   // 第一次要先读取包大小
        {
            // 第一次要先判断传入字节数是否小于4字节
            if(sizeNow < sizeof(quint32))   return;

            stream >> LastPacksize;
        }
        qDebug() << "sizeNow:"<< sizeNow;
        qDebug() << "sizePack:"<< LastPacksize;
        // 包不完整返回,防止出现半包
        if(sizeNow < LastPacksize - 4)     return;



        // 包已经完整,数据读取完毕
        qDebug() << "包已经完整,数据读取完毕";
        LastPacksize = 0;

        // 设置输出框内容
        QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        QByteArray dataFull;
        stream >> dataFull; // 读取完整数据
        QString prefix = dataFull.mid(0,4);
        if(prefix == "TXT:")
        {
            ui->tedtOutput->append(dateTime);
            QString textContent = dataFull.mid(4);
            ui->tedtOutput->append("<p>" + textContent + "</p><br>");
        }
        else if(prefix == "IMG:")
        {
            ui->tedtOutput->append(dateTime);
            QString index = QString::number(imageIndex);

            QFile file(index + ".png");
            file.open(QIODevice::WriteOnly);
            file.write(dataFull.mid(4));
            file.close();

            QString htmlTag = QString("<img src=\"%1\"></img><br>").arg(index + ".png");
            ui->tedtOutput->append(htmlTag);

            imageIndex++;
        }

        // 判断剩下的字节数,是否会有粘包的情况
        sizeNow = socket->bytesAvailable();
        qDebug() << "剩余等待读取的传入字节数 sizeNow=" << sizeNow;
    }while(sizeNow > 0);
}

void Widget::on_btnSend_clicked()
{
    QString qstrSend = ui->tedtInput->toPlainText();
    if(qstrSend.isEmpty())  return;

    QString input = "TXT:"+ qstrSend;
    QByteArray data = input.toLocal8Bit();

    // 封装数据包,加上包头
    QByteArray dataSend;    // 封装的数据包
    QDataStream stream(&dataSend,QIODevice::WriteOnly); // 定义一个数据流
    // 写入数据到数据流,格式为:数据包大小(32bit)+数据
    stream << (quint32)0 << data;
    // 移到流索引为0处, 用数据大小覆盖
    stream.device()->seek(0);
    stream << dataSend.size();

    tcpSocket.write(dataSend);
    ui->tedtInput->clear();
}

void Widget::on_btnImage_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(this,"请选择一张图片",".","Images (*.png *.xpm *.jpg)");
    if(imagePath.isEmpty()) return;

    QFile file(imagePath);
    file.open(QIODevice::ReadOnly);
    QByteArray data = "IMG:" + file.readAll();
    file.close();

    // 封装数据包,加上包头
    QByteArray dataSend;    // 封装的数据包
    QDataStream stream(&dataSend,QIODevice::WriteOnly); // 定义一个数据流
    // 写入数据到数据流,格式为:数据包大小(32bit)+数据
    stream << (quint32)0 << data;
    // 移到流索引为0处, 用数据大小覆盖
    stream.device()->seek(0);
    stream << dataSend.size();

    tcpSocket.write(dataSend);
}
