#pragma once

#include <QString>

enum class MODBUS_ANSWER_RESULT {
	OK				=	0,
	ERR_CONNECT		=	1,
	ERR_START		=	2,
	ERR_INTERFACE	=	3
};

struct serialPortCfg {
	QString				portName;			/// "/dev/ttyUSB0"
	int					parity;				/// QSerialPort::NoParity
	int					baudrate;			/// QSerialPort::Baud115200
	int					dataSize;			/// QSerialPort::Data8
	int					stopBit;			/// QSerialPort::OneStop
};

struct modbusSerialPacketCfg {
	serialPortCfg		p;
	int					clientAddress;
	int					startAddress;
	int					countRegister;
};

Q_DECLARE_METATYPE( MODBUS_ANSWER_RESULT );
Q_DECLARE_METATYPE( QVector< uint16_t > );
Q_DECLARE_METATYPE( serialPortCfg );
Q_DECLARE_METATYPE( modbusSerialPacketCfg );
