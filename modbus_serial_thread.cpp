#include "modbus_serial_thread.h"

ModbusThread::ModbusThread() : QObject( nullptr ) {
	qRegisterMetaType< MODBUS_ANSWER_RESULT >();
	qRegisterMetaType< modbusSerialCfg >();
	qRegisterMetaType< QVector< uint16_t > >();

	/*!
	 * Объект создается единожды, но перед каждой транзакцией
	 * конфигурируется заново согласно параметрам подключения.
	 */
	this->thread			=	new QThread();
	connect( this->thread, &QThread::started, this, &ModbusThread::startThread);
	this->moveToThread( thread );
	this->thread->start();
}

void ModbusThread::startThread	( void ) {
	this->modbus			=	new QModbusRtuSerialMaster( nullptr );
	this->m					=	new QMutex();
}

void ModbusThread::slotInitRead	(	QString					portName,
									int						parity,
									int						baudrate,
									int						dataSize,
									int						stopBit,
									int						clientAddress,
									int						startAddress,
									int						countRegister	) {
	/// Забираем ресурс.
	this->m->lock();

	/// Конфигурируем порт согласно параметрам.
	this->modbus->setConnectionParameter(	QModbusDevice::SerialPortNameParameter,
											portName	);

	this->modbus->setConnectionParameter(	QModbusDevice::SerialParityParameter,
											parity	);

	this->modbus->setConnectionParameter(	QModbusDevice::SerialBaudRateParameter,
											baudrate	);

	this->modbus->setConnectionParameter(	QModbusDevice::SerialDataBitsParameter,
											dataSize	);

	this->modbus->setConnectionParameter(	QModbusDevice::SerialStopBitsParameter,
											stopBit	);

	/// Пытаемся установить контакт.
	if ( this->modbus->connectDevice() != true ) {
		QVector< uint16_t >		emptyVector;
		emit signalFinishedRead( MODBUS_ANSWER_RESULT::ERR_CONNECT, emptyVector );
	}

	/// Описываем тип транзакции ( у нас всегда считывание ).
	QModbusDataUnit		dataUnit( QModbusDataUnit::RegisterType::InputRegisters, startAddress, countRegister );

	/// Ставим задачу отправить запрос и получить ответ.
	if ( auto *reply = this->modbus->sendReadRequest( dataUnit, clientAddress ) ) {
		/// Дальнейшие действия в прерывании по окончании прием-переадчи.
		connect( reply, &QModbusReply::finished, this, &ModbusThread::readyReadModbusPacket );
	} else {
		QVector< uint16_t >		emptyVector;
		emit signalFinishedRead( MODBUS_ANSWER_RESULT::ERR_START, emptyVector );
	}
}

void ModbusThread::readyReadModbusPacket ( void ) {
	MODBUS_ANSWER_RESULT			r;
	QVector< uint16_t >			regData;

	/// Забираем результат операции.
	auto reply = qobject_cast< QModbusReply* >( sender() );

	/// Все прошло удачно?
	if ( reply->error() == QModbusDevice::NoError ) {
		/// Забираем результат.
		const QModbusDataUnit answer = reply->result();
		r			=	MODBUS_ANSWER_RESULT::OK;
		regData		=	answer.values();

	} else {
		r			=	MODBUS_ANSWER_RESULT::ERR_INTERFACE;
	}

	reply->deleteLater();
	emit signalFinishedRead( r, regData );

	/// Ресурс свободен.
	this->m->unlock();
}

