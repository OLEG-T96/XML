#include "mainwindow.h"
#include <QApplication>
#include <checker.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Checker window;
    window.setWindowTitle("XML Checker");
    window.show();
    return a.exec();
}
