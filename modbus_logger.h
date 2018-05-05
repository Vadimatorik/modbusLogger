#pragma once

#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QAbstractItemModel>
#include <QBitArray>
#include <QObject>
#include <QSerialPort>
#include <QStandardItemModel>
#include <QtCore/QVariant>
#include <QUrl>
#include <QString>
#include <QMutex>
#include <QSemaphore>

struct modbusSerialCfg {
	QString				portName;			/// "/dev/ttyUSB0"
	int					parity;				/// QSerialPort::NoParity
	int					baudrate;			/// QSerialPort::Baud115200
	int					dataSize;			/// QSerialPort::Data8
	int					stopBit;			/// QSerialPort::OneStop
};

/*!
 * Список возвращаемых значений из
 * методов класса ModBusLogger.
 */
enum class MODBUS_LOGGER_RESULT {
	OK				=	0,
	ERR_CONNECT		=	1,
	ERR_START		=	2,
};

class ModBusLogger : public QObject {
	Q_OBJECT

public:
	ModBusLogger											(	QObject* parent = nullptr	);

	MODBUS_LOGGER_RESULT	readDataFromSerial				(	modbusSerialCfg&		portCfg,
																int						clientAddress,
																int						startAddress,
																int						countRegister,
																uint16_t*				returnData	);

private slots:
	void	readyReadModbusPacket 		( void );

private:
	/*!
	 * Метод настраивает объект modbus согласно
	 * данным из входной структуры для работы по serial порту.
	 */
	void	setCfgModbusSerial								(	modbusSerialCfg&		portCfg	);

	/*!
	 * Основной объект взаимодействия с modbus шиной.
	 * Конфигурируется заново перед каждой транзакцией.
	 */
	QModbusClient*						modbus;

	/*!
	 * Mutex забирается на время запроса и ответа.
	 */
	QMutex*								m;

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
	MODBUS_LOGGER_RESULT				resultFromIsr;

};
