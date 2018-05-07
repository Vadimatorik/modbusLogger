#include "modbus_low_lavel.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDate>

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

bool ModBusLowLavel::checkModbusParam ( const modbusSerialPacketCfg& packetCfg ) {
	/// Флаг надобности инициализации объекта modbus.
	bool r	=	false;

	bool rReconnect = false;
	/// Если порт уже был настроен, смотрим параметры предыдущего подключения.
	/// Если они совпадают - ничего повторно настраивать не будем.
	if ( this->modbus->state() == QModbusDevice::ConnectedState ) {
		if ( this->modbus->connectionParameter( QModbusDevice::SerialPortNameParameter )	!= packetCfg.p.portName )
			r = true;

		if ( this->modbus->connectionParameter( QModbusDevice::SerialParityParameter )		!= packetCfg.p.parity )
			r = true;

		if ( this->modbus->connectionParameter( QModbusDevice::SerialBaudRateParameter )	!= packetCfg.p.baudrate )
			r = true;

		if ( this->modbus->connectionParameter( QModbusDevice::SerialDataBitsParameter )	!= packetCfg.p.dataSize )
			r = true;

		if ( this->modbus->connectionParameter( QModbusDevice::SerialStopBitsParameter )	!= packetCfg.p.stopBit )
			r = true;

	} else {
		/// Порт не был подключен, инициализировать надо.
		r = true;
	}

	return r;
}

bool ModBusLowLavel::initPort ( const modbusSerialPacketCfg& packetCfg ) {
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
	return this->modbus->connectDevice();
}

bool ModBusLowLavel::reinitPort( const modbusSerialPacketCfg& packetCfg ) {
	if ( this->checkModbusParam( packetCfg ) == false )
		return true;					/// Инициализация порта не требуется.

	return this->initPort( packetCfg );
}

void ModBusLowLavel::slotReadData (	const modbusSerialPacketCfg	packetCfg	) {
	this->waitFreeResurse();			/// Ждем возможности запросить данные.

	/// Если не удалось инициализировать порт.
	if ( this->reinitPort( packetCfg ) != true ) {
		/// Выводим информацию о порте, который не удалось инициализировать.
		qDebug() << "Port was not reinit! Detail:" << endl;
		qDebug() << '\t' <<"Date:\t"			<< QDate::currentDate()	<< endl;
		qDebug() << '\t' <<"Baudrate:\t"		<< packetCfg.p.baudrate	<< endl;
		qDebug() << '\t' <<"DataSize:\t"		<< packetCfg.p.dataSize	<< endl;
		qDebug() << '\t' <<"Parity:\t"			<< packetCfg.p.parity	<< endl;
		qDebug() << '\t' <<"Port name:\t"		<< packetCfg.p.portName	<< endl;
		qDebug() << '\t' <<"Stop bits:\t"		<< packetCfg.p.stopBit	<< endl;
		qDebug() << endl;

		/// Ресурс освобожден.
		this->fBusy = false;

		return;
	}

	/// Описываем тип транзакции ( у нас всегда считывание ).
	QModbusDataUnit	dataUnit(	QModbusDataUnit::RegisterType::InputRegisters,
								packetCfg.startAddress,
								packetCfg.countRegister	);

	/// Ставим задачу отправить запрос.
	if ( auto *reply = this->modbus->sendReadRequest( dataUnit, packetCfg.clientAddress ) ) {
		/// По окончании запроса вызвать обработчик.
		connect( reply, &QModbusReply::finished, this, &ModBusLowLavel::slotFinishedRead );
	} else {							/// Не удалось инициировать запрос.
		/// Выводим информацию о запросе, который упал.
		qDebug() << "Read request was not start! Detail:" << endl;
		qDebug() << '\t' <<"Date:\t"			<< QDate::currentDate()		<< endl;
		qDebug() << '\t' <<"Start address:\t"	<< packetCfg.startAddress	<< endl;
		qDebug() << '\t' <<"Count register:\t"	<< packetCfg.countRegister	<< endl;
		qDebug() << '\t' <<"Client address:\t"	<< packetCfg.clientAddress	<< endl;
		qDebug() << endl;

		/// Ресурс освобожден.
		this->fBusy = false;
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
