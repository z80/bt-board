
#include <QtGui>
#include "serial.h"

int main( int argc, char * argv[] )
{
    QApplication app( argc, argv );

    Serial * ser = new Serial();
    ser->show();
    int res = app.exec();

    ser->deleteLater();
    return res;
}
