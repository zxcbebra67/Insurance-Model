// Порхай как бабочка, жаль что петух
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Insurance Company Simulator");
    w.show();
    return a.exec();
}
