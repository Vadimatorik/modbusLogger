#include "modbus_logger.h"

ModbusLogger::ModbusLogger() {
	this->mbll								=	new ModBusLowLavel;
	this->itemCfg							=	new ModbusLoggerItemCfg;

	itemCfg->mb								=	mbll;

	itemCfg->portCfg.portName				=	QString( "/dev/ttyUSB0" );
	itemCfg->portCfg.parity					=	QSerialPort::NoParity;
	itemCfg->portCfg.baudrate				=	QSerialPort::Baud115200;
	itemCfg->portCfg.dataSize				=	QSerialPort::Data8;
	itemCfg->portCfg.stopBit				=	QSerialPort::OneStop;
	itemCfg->clientAddress					=	0x01;
	itemCfg->startAddress					=	0x00;
	itemCfg->countRegister					=	5;
	itemCfg->period							=	1000;

	ModbusLoggerItem*	item				=	new	ModbusLoggerItem( itemCfg );

	item->start();

}

void ModbusLogger::start ( void ) {

}
