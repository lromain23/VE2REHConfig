#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon icon(":/icons/icon.png");
    if (icon.isNull()) {
        qDebug() << "Error: The icon image failed to load!";
    } else {
        a.setWindowIcon(icon);
    }
    MainWindow w;
    w.show();
    return a.exec();
}
