#include "modbus_logger_item.h"

ModbusLoggerItem::ModbusLoggerItem ( const ModbusLoggerItemCfg* const cfg )
	: QObject( nullptr ), cfg( cfg ) {
	qRegisterMetaType< MODBUS_ANSWER_RESULT >();
	qRegisterMetaType< modbusSerialCfg >();
	qRegisterMetaType< QVector< uint16_t > >();

	/// Настраиваем срабатывание таймера с заданным периодом.
	this->timer	= new QTimer( this );
	connect( this->timer, SIGNAL( timeout( void ) ), this, SLOT( timeout( void ) ) );

	/// Выделяем память под хранилище.
	this->inputRegData		=	new uint16_t[ cfg->countRegister ];
}

void ModbusLoggerItem::start ( void ) {
	this->timer->start( this->cfg->period );
}

void ModbusLoggerItem::timeout ( void ) {
	MODBUS_ANSWER_RESULT	r;

	r	=	this->cfg->mb->readDataFromSerial(	this->cfg->portCfg,
												this->cfg->clientAddress,
												this->cfg->startAddress,
												this->cfg->countRegister,
												this->inputRegData	);
	if ( r == MODBUS_ANSWER_RESULT::OK ) {
	} else {
	}
}
