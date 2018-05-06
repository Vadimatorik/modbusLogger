#include "modbus_logger_item.h"

ModbusLoggerItem::ModbusLoggerItem( ModBusLowLavel* const mb, const modbusSerialPacketCfg packetCfg )
	: QObject( nullptr ), mb( mb ), packetCfg( packetCfg ) {
	qRegisterMetaType< MODBUS_ANSWER_RESULT >();
	qRegisterMetaType< serialPortCfg >();
	qRegisterMetaType< QVector< uint16_t > >();

	/// Настраиваем срабатывание таймера с заданным периодом.
	this->timer	= new QTimer( this );
	connect( this->timer, SIGNAL( timeout( void ) ), this, SLOT( timeout( void ) ) );

	connect( this,
			 SIGNAL( signalReadData( const modbusSerialPacketCfg ) ),
			 this->mb,
			 SLOT( slotReadData( const modbusSerialPacketCfg ) ) );


}

void ModbusLoggerItem::start ( void ) {
	this->timer->start( this->packetCfg.period );
}

void ModbusLoggerItem::timeout ( void ) {
	MODBUS_ANSWER_RESULT	r;

	emit this->signalReadData( this->packetCfg );

	if ( r == MODBUS_ANSWER_RESULT::OK ) {
	} else {
	}
}
