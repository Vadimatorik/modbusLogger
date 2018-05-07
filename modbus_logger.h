#pragma once

#include "modbus_logger_item.h"

#include <QtGlobal>
#include <QXmlStreamReader>
#include <QFile>
#include <QString>
#include <qxmlstream.h>

class ModbusLogger : public QObject {
	Q_OBJECT

public:
	ModbusLogger( char* fileCfgPath );

	void	start				( void );

signals:
	void	operate				( void );

private:
	/// Парсинг XML.
	void	waitEmdTeg			( void );
	int		readBlockXML		( QVector< ModbusLoggerItem* >* obj );
	int		readFileCfg			( QVector< ModbusLoggerItem* >* obj );

	ModBusLowLavel*			mbll;
	ModbusLoggerItemCfg*	itemCfg;
	serialPortCfg*			portCfg;
	QXmlStreamReader*		xmlSR;

	QString					fileCfg;

	QThread					workerThread;
};
