#include "modbus_logger.h"

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

ModBusLogger::ModBusLogger ( QObject* parent ) : QObject( parent ) {
	/*!
	 * Объект создается единожды, но перед каждой транзакцией
	 * конфигурируется заново согласно параметрам подключения.
	 */
	this->modbus	=	new QModbusRtuSerialMaster( nullptr );

	/*!
	 * Эксклюзивный доступ к modbus.
	 */
	this->m			=	new	QMutex();

	/*!
	 * Выдается по окончании приема.
	 */
	this->s			=	new QSemaphore();
}


void ModBusLogger::setCfgModbusSerial ( modbusSerialCfg& portCfg ) {
	this->modbus->setConnectionParameter(	QModbusDevice::SerialPortNameParameter,
											portCfg.portName	);

	this->modbus->setConnectionParameter(	QModbusDevice::SerialParityParameter,
											portCfg.parity	);

	this->modbus->setConnectionParameter(	QModbusDevice::SerialBaudRateParameter,
											portCfg.baudrate	);

	this->modbus->setConnectionParameter(	QModbusDevice::SerialDataBitsParameter,
											portCfg.dataSize	);

	this->modbus->setConnectionParameter(	QModbusDevice::SerialStopBitsParameter,
											portCfg.stopBit	);
}

MODBUS_LOGGER_RESULT ModBusLogger::readDataFromSerial (	modbusSerialCfg&		portCfg,
														int						clientAddress,
														int						startAddress,
														int						countRegister,
														uint16_t*				returnData	) {
	/*!
	 * Забираем на время запроса и ответа.
	 */
	this->m->lock();

	/// Конфигурируем порт согласно параметрам.
	this->setCfgModbusSerial( portCfg );

	/// Пытаемся установить контакт.
	if ( this->modbus->connectDevice() != true )
		return MODBUS_LOGGER_RESULT::ERR_CONNECT;

	/// Показываем, куда класть данные, когда придет пакет.
	this->returnData	=	returnData;

	/// Описываем тип транзакции ( у нас всегда считывание ).
	QModbusDataUnit		dataUnit( QModbusDataUnit::RegisterType::InputRegisters, startAddress, countRegister );

	/// Ставим задачу отправить запрос и получить ответ.
	if ( auto *reply = this->modbus->sendReadRequest( dataUnit, clientAddress ) ) {
		/// Дальнейшие действия в прерывании по окончании прием-переадчи.
		connect( reply, &QModbusReply::finished, this, &ModBusLogger::readyReadModbusPacket );
	} else {
		/// Непредвиденная ситуация.
		this->modbus->disconnectDevice();
		this->m->unlock();
		return MODBUS_LOGGER_RESULT::ERR_START;
	}

	/// Ждем окончания передачи
	/// (прерывание может произойти по таймауту внутри modbus).
	//this->s->tryAcquire( 1, -1 );

	/// Обязательно освобождаем ресурс.
	//this->modbus->disconnectDevice();
	//this->m->unlock();

	return this->resultFromIsr;
}

void ModBusLogger::readyReadModbusPacket ( void ) {
	/// Забираем результат операции.
	auto reply = qobject_cast< QModbusReply* >( sender() );

	/// Все прошло удачно?
	if ( reply->error() == QModbusDevice::NoError ) {
		/// Забираем результат.
		const QModbusDataUnit answer = reply->result();

		/// Забираем данные.
		for ( uint i = 0; i < answer.valueCount(); i++ )
			this->returnData[ i ]	=	answer.value( i );

	} else {
		while(1);
	}


	reply->deleteLater();
	this->s->release( 1 );
}
