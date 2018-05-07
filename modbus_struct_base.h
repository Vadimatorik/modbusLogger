#pragma once

#include <QString>

enum class MODBUS_ANSWER_RESULT {
	OK				=	0,
	ERR_CONNECT		=	1,
	ERR_START		=	2,
	ERR_INTERFACE	=	3
};

/*!
 * Структура конфигурации последовательно
 * порта для modbus.
 */
struct serialPortCfg {
	QString				portName;			/// "/dev/ttyUSB0"
	int					parity;				/// QSerialPort::NoParity
	int					baudrate;			/// QSerialPort::Baud115200
	int					dataSize;			/// QSerialPort::Data8
	int					stopBit;			/// QSerialPort::OneStop
};

/*!
 * Параметры пакета, запрашиваемые по
 * последовательному modbus.
 */
struct modbusSerialPacketCfg {
	QString				deviceName;
	int					period;
	serialPortCfg		p;					/// Параметры конфигурации последовательного порта.
	int					clientAddress;
	int					startAddress;
	int					countRegister;
	quint16*			returnData;			/// Приходящие данные кладутся сюда.
};

Q_DECLARE_METATYPE( MODBUS_ANSWER_RESULT );
Q_DECLARE_METATYPE( QVector< uint16_t > );
Q_DECLARE_METATYPE( serialPortCfg );
Q_DECLARE_METATYPE( modbusSerialPacketCfg );
Q_DECLARE_METATYPE( modbusSerialPacketCfg* );
