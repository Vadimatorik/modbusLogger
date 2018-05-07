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
	/*!
	 * Инициализирует заново modbus, если конфигурация
	 * последнего не совпадает с ранее установленными параметрами
	 * или modbus не был ранее инициализирован.
	 *
	 * \param[in]	packetCfg		-	параметры конфигурируемого порта.
	 * \return		{	True	-	объект был успешно настроен или настройка не требовалась.
	 *					False	-	в противном случае.	}
	 */
	bool	reinitPort						(	const modbusSerialPacketCfg&	packetCfg	);

	/*!
	 * Сбрасывает параметры предыдущего соединения объекта modbus
	 * и инициализирует его заново согласно требуемым параметрам.
	 *
	 * \param[in]	packetCfg		-	параметры конфигурируемого порта.
	 *
	 * \return		{	True	-	объект был успешно настроен.
	 *					False	-	в противном случае.	}
	 */
	bool	initPort						(	const modbusSerialPacketCfg&	packetCfg	);

	/*!
	 * Проверят, требуется ли повторная инициализация объекта
	 * modbus новыми параметрами или нет.
	 *
	 * \param[in]	packetCfg		-	параметры, которым должен соответствовать порт.
	 *
	 * \return		{	True	-	объект нуждается в повторной настройке новыми параметрами.
	 *					False	-	в противном случае.	}
	 */
	bool	checkModbusParam				(	const modbusSerialPacketCfg&	packetCfg	);

	volatile bool							fBusy;
	QModbusClient*							modbus;

	void waitFreeResurse	( void );
};
