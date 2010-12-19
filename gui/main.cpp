#include "gui.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gui w;
    w.show();
    return a.exec();
}
