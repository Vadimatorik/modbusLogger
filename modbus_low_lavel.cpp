#include "modbus_low_lavel.h"

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

ModBusLowLavel::ModBusLowLavel ( QObject* parent ) : QObject( parent ) {
	qRegisterMetaType< MODBUS_ANSWER_RESULT >();
	qRegisterMetaType< modbusSerialCfg >();
	qRegisterMetaType< QVector< uint16_t > >();

	this->mbt			=	new ModbusThread();

	/// Отправка в поток задания запроса регистров.
	connect(	this,
				&ModBusLowLavel::signalInitRead,
				this->mbt,
				&ModbusThread::slotInitRead	);

	/// Прием ответа из потока.
	connect( this->mbt,
			 SIGNAL( signalFinishedRead( MODBUS_ANSWER_RESULT, QVector< uint16_t > ) ),
			 this,
			 SLOT( slotFinishedRead( MODBUS_ANSWER_RESULT, QVector< uint16_t > ) ) );


	/*!
	 * Эксклюзивный доступ к modbus.
	 */
	this->m			=	new	QMutex();

	/*!
	 * Выдается по окончании приема.
	 */
	this->s			=	new QSemaphore();

	qRegisterMetaType< MODBUS_ANSWER_RESULT >();
}

MODBUS_ANSWER_RESULT ModBusLowLavel::readDataFromSerial (	modbusSerialCfg			portCfg,
															int						clientAddress,
															int						startAddress,
															int						countRegister,
															uint16_t*				returnData	) {
	/*!
	 * Забираем на время запроса и ответа.
	 */
	this->m->lock();

	/// Куда класть по прибытии.
	this->returnData			=	returnData;

	emit this->signalInitRead(	portCfg.portName,
								portCfg.parity,
								portCfg.baudrate,
								portCfg.dataSize,
								portCfg.stopBit,
								clientAddress,
								startAddress,
								countRegister );

	/// Ждем окончания передачи
	/// (прерывание может произойти по таймауту внутри modbus).
	//this->s->tryAcquire( 1, -1 );

	/// Освобождаем ресурс.
	this->m->unlock();

	return this->resultFromIsr;
}

void ModBusLowLavel::slotFinishedRead (	MODBUS_ANSWER_RESULT		result,
										QVector< uint16_t >		reg	) {
	/// забираем результаты.
	this->resultFromIsr	=	result;
	for ( int i = 0; i < reg.count(); i++ )
		returnData[ i ]	=	reg.value( i );

	/// Сообщаем что результат здесь.
	this->s->release( 1 );
}
