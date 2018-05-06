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

	/// Пытаемся установить контакт.
	if ( this->modbus->connectDevice() != true ) {		
		this->modbus->disconnectDevice();
		this->fBusy		= false;
		return;
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

void ModBusLowLavel::slotFinishedRead (	void ) {
	/// Забираем результат операции.
	auto reply = qobject_cast< QModbusReply* >( sender() );

	/// Все прошло удачно?
	if ( reply->error() == QModbusDevice::NoError ) {
		const QModbusDataUnit answer = reply->result();
	} else {
	}

	reply->deleteLater();
	this->fBusy = false;
}
