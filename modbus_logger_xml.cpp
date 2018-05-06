#include "modbus_logger.h"

void ModbusLogger::waitEmdTeg ( void ) {
	this->xmlSR->readNext();
	while( ( this->xmlSR->tokenType() != QXmlStreamReader::EndElement ) && ( !this->xmlSR->atEnd() ) ) {
		this->xmlSR->readNext();
	}
}

int ModbusLogger::readBlockXML ( QVector< ModbusLoggerItem* >* obj ) {
	modbusSerialPacketCfg objCfg;

	while( !this->xmlSR->atEnd() && !this->xmlSR->hasError()  ) {
		this->xmlSR->readNext();
		if ( this->xmlSR->tokenType() == QXmlStreamReader::StartElement ) {
			while( !( this->xmlSR->tokenType() == QXmlStreamReader::EndElement && ( this->xmlSR->name() == "item" ) ) ) {
				/// Участвует ли в опросе блок.
				if ( this->xmlSR->name() == "state" ) {
					int state = this->xmlSR->readElementText().toUInt( nullptr, 10 );
					if ( state == 0 )
						return 1;			/// Не участвует.
				}

				/// Имя устройства, которое опрашивается.
				if ( this->xmlSR->name() == "name_device" ) {
					objCfg.deviceName = this->xmlSR->readElementText();
				}

				/// Пока только COM.

				/// Имя порта в системе.
				if ( this->xmlSR->name() == "port_name" ) {
					objCfg.p.portName = this->xmlSR->readElementText();
				}

				/// Четность.
				if ( this->xmlSR->name() == "parity" ) {
					objCfg.p.parity = this->xmlSR->readElementText().toUInt( nullptr, 10 );
				}

				/// Скорость.
				if ( this->xmlSR->name() == "baudrate" ) {
					objCfg.p.baudrate = this->xmlSR->readElementText().toUInt( nullptr, 10 );
				}

				/// Минимальная единица передачи.
				if ( this->xmlSR->name() == "data_size" ) {
					objCfg.p.dataSize = this->xmlSR->readElementText().toUInt( nullptr, 10 );
				}

				/// Количество бит остановки.
				if ( this->xmlSR->name() == "stop_bits" ) {
					objCfg.p.stopBit = this->xmlSR->readElementText().toUInt( nullptr, 10 );
				}

				/// Количество бит остановки.
				if ( this->xmlSR->name() == "client_address" ) {
					objCfg.clientAddress = this->xmlSR->readElementText().toUInt( nullptr, 10 );
				}

				/// Количество бит остановки.
				if ( this->xmlSR->name() == "period" ) {
					objCfg.period = this->xmlSR->readElementText().toUInt( nullptr, 10 );
				}

				/// Параметры объекта.
				if ( this->xmlSR->name() == "zone" ) {
					foreach( const QXmlStreamAttribute &attr, xmlSR->attributes() ) {
						if ( attr.name().toString() == "address" ) {
							/// Адрес.
							objCfg.startAddress = attr.value().toUInt( nullptr, 10 );
						}

						if ( attr.name().toString() == "size" ) {
							/// Количество регистров.
							objCfg.countRegister = attr.value().toUInt( nullptr, 10 );

							/// Создаем объект-опросник.
							ModbusLoggerItem*		item;
							item	=	new ModbusLoggerItem( this->mbll, objCfg );
							obj->append( item );
						}
					}
				}

				this->xmlSR->readNext();
			}
		}
	}

	return -1;
}


int ModbusLogger::readFileCfg ( QVector< ModbusLoggerItem* >* obj ) {
	QFile f( this->fileCfg );
	if ( !f.open(QFile::ReadOnly | QFile::Text) ) {
		return -1;
	}

	this->xmlSR->setDevice( &f );
	this->xmlSR->readNext();

	int	int_result;

	/// Вычитываем весь файл.
	while( !this->xmlSR->atEnd() && !this->xmlSR->hasError()  ) {
		QXmlStreamReader::TokenType token = this->xmlSR->readNext();

		if ( token == QXmlStreamReader::StartDocument ) {
			continue;
		}

		if ( token == QXmlStreamReader::StartElement ) {
			if ( this->xmlSR->name() == "modbus_cfg" ) {
				while( !(this->xmlSR->tokenType() == QXmlStreamReader::EndElement && ( this->xmlSR->name() == "modbus_cfg" )) ) {
					if ( this->xmlSR->name() == "item" ) {
						int_result = this->readBlockXML( obj );
						if ( int_result != 0 )	return int_result;
					}
					token = this->xmlSR->readNext();
				}
			}
		}
	}

	return 0;
}
