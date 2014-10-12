
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
        serial->setPortName( "/dev/ttyUSB0" );
        //serial->setPortName( "COM1" );
        serial->setBaudRate( BAUD9600 );
        //serial->setBaudRate( BAUD38400 );
        serial->setDataBits( DATA_8 );
        serial->setStopBits( STOP_1 );
        serial->setFlowControl( FLOW_OFF );
        serial->setParity( PAR_NONE );
        bool res = serial->open( QIODevice::ReadWrite );
        ui.logTe->appendPlainText( QString( "open result: %1" ).arg( res ? "true" : "false" ) );
        timer->start();
    }
    else
    {
        serial->close();
        timer->stop();
        ui.logTe->appendPlainText( "closed" );
    }
}

void Serial::rts()
{
    bool res = ui.rtsBtn->isChecked();
    serial->setRts( res );
    ui.logTe->appendPlainText( QString( "rts: %1" ).arg( res ? "true" : "false" ) );
}

void Serial::dtr()
{
    bool res = ui.dtrBtn->isChecked();
    serial->setDtr( res );
    ui.logTe->appendPlainText( QString( "dtr: %1" ).arg( res ? "true" : "false" ) );
}

void Serial::send()
{
    QString stri = ui.cmdLe->text();
    //stri.append( "\r\n\r\n" );
    QByteArray a = stri.toAscii();
    //a.append( '\r' );
    //a.append( '\n' );
    int cnt = serial->write( a );
    ui.logTe->appendPlainText( QString( "bytes written: %1" ).arg( cnt ) );
}

void Serial::timeout()
{
    int cnt = serial->bytesAvailable();
    QByteArray a;
    if ( cnt > 0 )
    {
        ui.logTe->appendPlainText( QString( "%1 bytes received:ui.logTe->appendPlainText( QString::fromAscii( a ) )" ).arg( cnt ) );
        a = serial->readAll();
        ui.logTe->appendPlainText( QString::fromAscii( a ) );
    }
}





