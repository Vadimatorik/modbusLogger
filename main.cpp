#include <QCoreApplication>
#include <stdio.h>
#include "modbus_logger.h"

int main ( int argc, char *argv[] ) {
	QCoreApplication	a( argc, argv );
	ModBusLogger		mbl;

	modbusSerialCfg		cfgPort;

	cfgPort.portName			=	QString( "/dev/ttyUSB0" );
	cfgPort.parity				=	QSerialPort::NoParity;
	cfgPort.baudrate			=	QSerialPort::Baud115200;
	cfgPort.dataSize			=	QSerialPort::Data8;
	cfgPort.stopBit				=	QSerialPort::OneStop;

	uint16_t		dataReturn[5];

	mbl.readDataFromSerial( cfgPort, 0x01, 0x00, 5, dataReturn );

	return a.exec();
}
