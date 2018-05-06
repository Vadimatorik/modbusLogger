#include <QCoreApplication>
#include "modbus_logger.h"

int main ( int argc, char *argv[] ) {
	QCoreApplication	a( argc, argv );
	ModbusLogger		mbl;
	mbl.start();
	return a.exec();
}
