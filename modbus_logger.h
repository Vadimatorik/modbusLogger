#pragma once

#include "modbus_logger_item.h"
#include <QThread>

class ModbusLogger : public QObject {
	Q_OBJECT

public:
	ModbusLogger();

	void	start				( void );

signals:
	void	operate				( void );

private:
	ModBusLowLavel*			mbll;
	ModbusLoggerItemCfg*	itemCfg;
	modbusSerialCfg*		portCfg;
	QThread					workerThread;
};
