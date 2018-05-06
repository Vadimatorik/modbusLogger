#pragma once

#include "modbus_serial_thread.h"
#include <QSemaphore>
#include <QMetaType>

class ModBusLowLavel : public QObject {
	Q_OBJECT

public:
	ModBusLowLavel											(	QObject* parent = nullptr	);

	/*!
	 *
	 */
	MODBUS_ANSWER_RESULT	readDataFromSerial				(	modbusSerialCfg			portCfg,
																int						clientAddress,
																int						startAddress,
																int						countRegister,
																uint16_t*				returnData		);
signals:
	/*!
	 *
	 */
	void	signalInitRead									(	QString					portName,
																int						parity,
																int						baudrate,
																int						dataSize,
																int						stopBit,
																int						clientAddress,
																int						startAddress,
																int						countRegister	);


private slots:
	void	slotFinishedRead								(	MODBUS_ANSWER_RESULT		result,
																QVector< uint16_t >		reg	);

private:
	/*!
	 * Modbus в отдельном потоке.
	 */
	ModbusThread*						mbt;

	/*!
	 * Semaphore выдается по окончании транзакции.
	 */
	QSemaphore*							s;

	/*!
	 * Адрес расположения данных, по приходе удачного пакета.
	 */
	uint16_t*							returnData;

	/*!
	 * Результат запроса-ответа modbus
	 * (выставляется в прерывании и
	 * отдается по выходе из).
	 */
	MODBUS_ANSWER_RESULT				resultFromIsr;

	/*!
	 * Mutex забирается на время запроса и ответа.
	 */
	QMutex*								m;
};
