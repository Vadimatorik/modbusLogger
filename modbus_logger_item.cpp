#include "modbus_logger_item.h"

ModbusLoggerItem::ModbusLoggerItem( ModBusLowLavel* const mb, modbusSerialPacketCfg packetCfg )
	: QObject( nullptr ), mb( mb ), packetCfg( packetCfg ) {
	qRegisterMetaType< MODBUS_ANSWER_RESULT >();
	qRegisterMetaType< serialPortCfg >();
	qRegisterMetaType< modbusSerialPacketCfg* >();
	qRegisterMetaType< QVector< uint16_t > >();


	/// Настраиваем срабатывание таймера с заданным периодом.
	this->timer	= new QTimer( this );
	connect( this->timer, SIGNAL( timeout( void ) ), this, SLOT( timeout( void ) ) );

	connect( this,
			 SIGNAL( signalReadData( modbusSerialPacketCfg* ) ),
			 this->mb,
			 SLOT( slotReadData( modbusSerialPacketCfg* ) ) );

	/// Сюда будут класться данные.
	this->packetCfg.returnData	=	new quint16[ this->packetCfg.countRegister ];
}

void ModbusLoggerItem::start ( void ) {
	this->timer->start( this->packetCfg.period );
}

void ModbusLoggerItem::timeout ( void ) {
	emit this->signalReadData( &this->packetCfg );
}
