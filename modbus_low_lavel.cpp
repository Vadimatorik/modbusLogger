#include "modbus_low_lavel.h"
#include <QCoreApplication>


ModBusLowLavel::ModBusLowLavel ( QObject* parent ) : QObject( parent ) {
	qRegisterMetaType< MODBUS_ANSWER_RESULT >();
	qRegisterMetaType< QVector< uint16_t > >();	
	qRegisterMetaType< modbusSerialPacketCfg >();

	this->modbus			=	new QModbusRtuSerialMaster( nullptr );
}

void ModBusLowLavel::waitFreeResurse	( void ) {
	/// Ждем пока отработает предыдущее обращение.
	while( this->fBusy ) {
		QCoreApplication::processEvents();
	}
	this->fBusy		= true;
}

void ModBusLowLavel::slotReadData (	const modbusSerialPacketCfg	packetCfg	) {
	this->waitFreeResurse();

	bool rReconnect = false;
	/// Если порт уже был настроен, смотрим параметры предыдущего подключения.
	/// Если они совпадают - ничего переподключать не будем.
	if ( this->modbus->state() == QModbusDevice::ConnectedState ) {
		if ( this->modbus->connectionParameter( QModbusDevice::SerialPortNameParameter ) != packetCfg.p.portName )
			rReconnect = true;

		if ( this->modbus->connectionParameter( QModbusDevice::SerialParityParameter ) != packetCfg.p.parity )
			rReconnect = true;

		if ( this->modbus->connectionParameter( QModbusDevice::SerialBaudRateParameter ) != packetCfg.p.baudrate )
			rReconnect = true;

		if ( this->modbus->connectionParameter( QModbusDevice::SerialDataBitsParameter ) != packetCfg.p.dataSize )
			rReconnect = true;

		if ( this->modbus->connectionParameter( QModbusDevice::SerialStopBitsParameter ) != packetCfg.p.stopBit )
			rReconnect = true;

	} else {
		rReconnect = true;	/// Порт не был подключен, инициализировать надо.
	}

	/// Если стараяконфигурация не подходит или ее нет вообще.
	if ( rReconnect == true ) {
		/// Конфигурируем порт согласно параметрам.
		this->modbus->setConnectionParameter(	QModbusDevice::SerialPortNameParameter,
												packetCfg.p.portName	);

		this->modbus->setConnectionParameter(	QModbusDevice::SerialParityParameter,
												packetCfg.p.parity	);

		this->modbus->setConnectionParameter(	QModbusDevice::SerialBaudRateParameter,
												packetCfg.p.baudrate	);

		this->modbus->setConnectionParameter(	QModbusDevice::SerialDataBitsParameter,
												packetCfg.p.dataSize	);

		this->modbus->setConnectionParameter(	QModbusDevice::SerialStopBitsParameter,
												packetCfg.p.stopBit	);

		this->modbus->setTimeout( 100 );
		/// Пытаемся установить контакт.
		if ( this->modbus->connectDevice() != true ) {
			this->modbus->disconnectDevice();
			this->fBusy		= false;
			return;
		}
	}

	/// Описываем тип транзакции ( у нас всегда считывание ).
	QModbusDataUnit		dataUnit( QModbusDataUnit::RegisterType::InputRegisters, packetCfg.startAddress, packetCfg.countRegister );

	/// Ставим задачу отправить запрос и получить ответ.
	if ( auto *reply = this->modbus->sendReadRequest( dataUnit, packetCfg.clientAddress ) ) {
		/// Дальнейшие действия в прерывании по окончании прием-переадчи.
		connect( reply, &QModbusReply::finished, this, &ModBusLowLavel::slotFinishedRead );
	} else {
		return;
	}
}


#include <iostream>

void ModBusLowLavel::slotFinishedRead (	void ) {
	/// Забираем результат операции.
	auto reply = qobject_cast< QModbusReply* >( sender() );

	/// Все прошло удачно?
	if ( reply->error() == QModbusDevice::NoError ) {
		const QModbusDataUnit answer = reply->result();
		std::cout << answer.value( 0 ) << '\t' << answer.value( 1 ) << std::endl;

	} else {
		std::cout << "Error!";
	}

	reply->deleteLater();
	this->fBusy = false;
}
