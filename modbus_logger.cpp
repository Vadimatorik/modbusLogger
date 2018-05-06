#include "modbus_logger.h"

ModbusLogger::ModbusLogger( char* fileCfgPath ) {
	/// Путь до файла с конфигурацией.
	this->fileCfg		=	QString( fileCfgPath );

	/// Объект сканирования XML.
	this->xmlSR			=	new QXmlStreamReader();

	/// ModBus протокол.
	this->mbll								=	new ModBusLowLavel;

	/// Получаем список устройств-адресов.
	QVector< ModbusLoggerItem* >		items;
	this->readFileCfg( &items );

	/// Запускаем все объекты опроса.
	for ( int l = 0; l < items.count(); l++ )
		items.at( l )->start();
}



void ModbusLogger::start ( void ) {

}
