#include "widget.h"
#include <QApplication>
#include <QStyleFactory>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    qApp->setStyle(QStyleFactory::create("Fusion"));
    Widget w;
    w.show();

    return a.exec();
}
