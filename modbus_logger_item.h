#pragma once

#include "modbus_low_lavel.h"
#include <QTimer>

/*!
 * Конфигурация объекта элемента modbus шины.
 */
struct ModbusLoggerItemCfg {
	ModBusLowLavel*			mb;					/// Объект modbus-а.
	modbusSerialCfg			portCfg;			/// Параметры подключения.
	int						clientAddress;		/// Адрес опрашиваемого устройства.
	int						startAddress;		/// Адрес, с которого спрашиваем.
	int						countRegister;		/// Количество регистров.
	int						period;				/// Период опроса кадра.
};

/*!
 * Класс предназначен для создания запрос-ответного "потока"
 * (поток не создается, но по сути работает так).
 *
 * Объект данного типа создается для каждого типа запроса
 * (каждого запрос).
 */
class ModbusLoggerItem : public QObject {
	Q_OBJECT

public:
	ModbusLoggerItem( const ModbusLoggerItemCfg* const cfg );

public slots:
	void			start			( void );

private slots:
	/*!
	 * Слот срабатывания таймера (требуется новый запрос).
	 */
	void			timeout			( void );

private:
	const ModbusLoggerItemCfg* const cfg;

	/// Таймер для опроса с заданным периодом.
	QTimer*				timer;

	/// Хранилище входных данных.
	uint16_t*			inputRegData;
};
