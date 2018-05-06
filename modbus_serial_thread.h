#pragma once

#include <QModbusDataUnit>
#include <QModbusRtuSerialMaster>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QSerialPort>
#include <QObject>
#include <QString>
#include <QVector>
#include <QThread>
#include <QMutex>
#include <QUrl>

#include "modbus_struct_base.h"

/*!
 * Оболочка для класса QModbusRtuSerialMaster,
 * которая обеспечивает работу объекта класса в отдельном потоке.
 */
class ModbusThread : public QObject {
	Q_OBJECT

public:
	ModbusThread();

public slots:
	void	slotInitRead				(	QString					portName,
											int						parity,
											int						baudrate,
											int						dataSize,
											int						stopBit,
											int						clientAddress,
											int						startAddress,
											int						countRegister	);

signals:
	void	signalFinishedRead			(	MODBUS_ANSWER_RESULT		result,
											QVector< uint16_t >		reg	);

private slots:
	void	readyReadModbusPacket 		( void );
	void	startThread					( void );

private:

	/*!
	 * В этом потоке будет крутиться объект.
	 */
	QThread*							thread;

	/*!
	 * Основной объект взаимодействия с modbus шиной.
	 * Конфигурируется заново перед каждой транзакцией.
	 */
	QModbusClient*						modbus;

	/*!
	 * Mutex забирается на время запроса и ответа.
	 */
	QMutex*								m;
};

