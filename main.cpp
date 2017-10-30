#include <QApplication>
#include <QMessageBox>

#include "torbridgescollector.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TorBridgesCollector w;
    w.show();
    return a.exec();
}
