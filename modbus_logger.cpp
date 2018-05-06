#include "modbus_logger.h"

ModbusLogger::ModbusLogger() {
	this->mbll								=	new ModBusLowLavel;

	modbusSerialPacketCfg*					itemCfg;
	itemCfg	=	new modbusSerialPacketCfg();

	itemCfg->p.portName				=	QString( "/dev/ttyUSB0" );
	itemCfg->p.parity				=	QSerialPort::NoParity;
	itemCfg->p.baudrate				=	QSerialPort::Baud115200;
	itemCfg->p.dataSize				=	QSerialPort::Data8;
	itemCfg->p.stopBit				=	QSerialPort::OneStop;
	itemCfg->clientAddress			=	0x01;
	itemCfg->startAddress			=	0x00;
	itemCfg->countRegister			=	5;

	ModbusLoggerItem*	item				=	new	ModbusLoggerItem( mbll, *itemCfg );
	ModbusLoggerItem*	item1				=	new	ModbusLoggerItem( mbll, *itemCfg );

	item->start( 1000 );
	item1->start( 500 );

}

void ModbusLogger::start ( void ) {

}
