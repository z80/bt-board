#include <QtGui>

#ifndef __SERIAL_H_
#define __SERIAL_H_

#include "ui_serial.h"
#include "qextserialport.h"


class Serial: public QWidget
{
    Q_OBJECT
public:
    Serial( QWidget * parent = 0 );
    ~Serial();

private slots:
    void open();
    void rts();
    void dtr();
    void send();
    void timeout();
private:
    Ui_serialWnd ui;
    QextSerialPort * serial;
    QTimer         * timer;
};


#endif




