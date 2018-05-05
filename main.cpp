#include <QCoreApplication>

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

#include <stdio.h>


QModbusDataUnit readRequest() {
	const auto table = QModbusDataUnit::RegisterType::InputRegisters;
	int startAddress = 5;
	return QModbusDataUnit(table, startAddress, 0x10);
}


int main ( int argc, char *argv[] ) {
	QCoreApplication a(argc, argv);

	QModbusClient*		modbus;

	modbus = new QModbusRtuSerialMaster( nullptr );

	modbus->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
		"ttyUSB0" );
	modbus->setConnectionParameter(QModbusDevice::SerialParityParameter,
		QSerialPort::EvenParity);
	modbus->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
		QSerialPort::Baud9600);
	modbus->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
		QSerialPort::Data8);
	modbus->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
		QSerialPort::OneStop);

	if ( !modbus->connectDevice() ) {
		while(1);
	} else {
		auto* reply = modbus->sendReadRequest(readRequest(), 0x10 );

	}

	return a.exec();
}
