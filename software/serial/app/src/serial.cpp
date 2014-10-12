
#include "serial.h"

Serial::Serial( QWidget * parent )
  : QWidget( parent )
{
    ui.setupUi( this );
    serial = new QextSerialPort();
    connect( ui.openBtn, SIGNAL(clicked()), this, SLOT(open()) );
    connect( ui.rtsBtn,  SIGNAL(clicked()), this, SLOT(rts()) );
    connect( ui.dtrBtn,  SIGNAL(clicked()), this, SLOT(dtr()) );
    connect( ui.sendBtn, SIGNAL(clicked()), this, SLOT(send()) );

    timer = new QTimer( this );
    timer->setInterval( 1000 );
    connect( timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

Serial::~Serial()
{
    serial->deleteLater();
}

void Serial::open()
{
    if ( ui.openBtn->isChecked() )
    {
        serial->setBaudRate( BAUD9600 );
        serial->setDataBits( DATA_8 );
        serial->setStopBits( STOP_1 );
        serial->setFlowControl( FLOW_OFF );
        serial->setParity( PAR_NONE );
        serial->open();
    }
    else
        serial->close();
}

void Serial::rts()
{
    bool res = ui.rtsBtn->isChecked();
    serial->setRts( res );
}

void Serial::dtr()
{
    bool res = ui.dtrBtn->isChecked();
    serial->setDtr( res );
}

void Serial::send()
{
    QString stri = ui.cmdLe->text();
    stri.append( "\r\n" );
    QByteArray a = stri.toAscii();
    int cnt = serial->write( a );
}

void Serial::timeout()
{
    int cnt = serial->bytesAvailable();
    QByteArray a;
    a = serial->readAll();
    ui.logTe->appendPlainText( QString::fromAscii( a ) );
}





