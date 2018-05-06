#pragma once

#include <QModbusDataUnit>
#include <QAbstractItemModel>
#include <QBitArray>
#include <QObject>
#include <QSerialPort>
#include <QUrl>
#include <QString>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QUrl>
#include <QModbusDataUnit>
#include <QThread>
#include <QSemaphore>
#include <QMetaType>

#include "modbus_struct_base.h"

class ModBusLowLavel : public QObject {
	Q_OBJECT

public:
	ModBusLowLavel							( QObject* parent = nullptr );

public slots:
	void	slotReadData					( const modbusSerialPacketCfg	packetCfg );

private slots:
	void	slotFinishedRead				( void );

private:
	volatile bool							fBusy;
	QModbusClient*							modbus;

	void waitFreeResurse	( void );
};
