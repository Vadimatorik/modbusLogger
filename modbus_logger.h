#pragma once

#include "modbus_logger_item.h"

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
	serialPortCfg*			portCfg;
	QThread					workerThread;
};
