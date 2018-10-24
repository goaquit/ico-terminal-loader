#include <QtCore/QCoreApplication>
#include "Server.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	TerminalServer::Server server;
	server.start();

	return a.exec();
}
