#include <QCoreApplication>
#include "modbus_logger.h"

int main ( int argc, char *argv[] ) {
	QCoreApplication	a( argc, argv );
	ModbusLogger		mbl( argv[ 1 ] );
	mbl.start();
	return a.exec();
}
