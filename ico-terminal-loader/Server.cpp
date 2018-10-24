#include "Server.h"

#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <memory>
#include <iostream>
#include <QDataStream>

namespace TerminalServer
{


Server::Server(QObject * parent) : server(new QTcpServer(parent))
{
	connect(server, &QTcpServer::newConnection, this, &Server::newConnection);
}

Server::~Server()
{
	server->close();

	qDeleteAll(clients.begin(), clients.end());
	clients.clear();

	emit closed();
}

void Server::start(quint16 port)
{
	if (!server || !server->isListening())
	{
		server->listen(QHostAddress::Any, port);
		std::cout << "Server starting. Using port: " << port << std::endl;


		loader.setStorageDepth(&storage);
		loader.start();
	}
}

void Server::newConnection()
{
	auto client = server->nextPendingConnection();
	clients.append(client);

	connect(client, &QTcpSocket::readyRead, this, &Server::readData);
	connect(client, &QTcpSocket::disconnected, this, &Server::socketDisconnected);

}

void Server::readData()
{
	auto client = qobject_cast<QTcpSocket *>(sender());
	if (!client) return;

	while (!client->atEnd())
	{
		const auto byteArray = client->readAll();

		QString inputRequest(byteArray);

		// split request
		auto listRequest = inputRequest.split("#", QString::SkipEmptyParts);

		for (auto &request : listRequest)
		{
			// find pair key
			uint limit = 0;
			QRegExp rx("(.+)[:;+_](\\d+)");
			if (rx.indexIn(request) != -1)
			{
				limit = rx.cap(2).toUInt();
				request = rx.cap(1);
			}

			const auto depthOrder = storage.getDepthOrder(request);

			if (depthOrder)
			{
				sendDepthOrder(depthOrder, client, request, limit);
			}
			else
			{
				std::cout << "ERROR: Data request incorrect " << request.toStdString() << std::endl;
			}

		}
	}
	
}

void Server::socketDisconnected()
{
	QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());

	if (!client) return;

#if QT_VERSION > QT_VERSION_CHECK(5,4,0)
	clients.removeAll(client);
#else
    int index = -1;
    while( (index = clients.indexOf(client)) != -1)
    {
        clients.remove((index));
    }
#endif

	client->deleteLater();
}

void Server::sendDepthOrder(const Entity::DepthOrder* depthOrder, QTcpSocket * client, QString index, uint limit)
{
	//QMutexLocker lock(&mtx);
	QByteArray block;
	QDataStream sendStream(&block, QIODevice::WriteOnly);

	quint16 countBids = 0;
	quint16 countAsks = 0;

	union ConverterDouble
	{
		double d;
		char   bytes[8];
	};

	ConverterDouble amountBids; 
	ConverterDouble amountAsks; 

	amountBids.d = depthOrder->quantityBids();
	amountAsks.d = depthOrder->quantityAsks();

	quint32 blockSize = 0;
	quint8  marker = 0xff;

	sendStream
		<< blockSize  // package size
		<< marker     // type: "Depth Order"
		<< countBids
		<< countAsks
		<< amountBids.d
		<< amountAsks.d;
	
	quint16 sizeIndex = index.length();
	sendStream << sizeIndex;
	sendStream.writeRawData(index.toStdString().c_str(), index.size());
	
	// send bids/asks data 
	const auto bidsLimit = (!limit) ? depthOrder->bids().size() : limit;
	for (const auto &order : depthOrder->bids())
	{
		if (countBids >= bidsLimit) break;
		sendStream << order.getPrice();
		sendStream << order.getAmount();

		countBids++;
	}

	const auto asksLimit = (!limit) ? depthOrder->asks().size() : limit;
	for (const auto &order : depthOrder->asks())
	{
		if (countAsks >= asksLimit) break;
		sendStream << order.getPrice();
		sendStream << order.getAmount();

		countAsks++;
	}

	sendStream << quint16(0xffff); // block end signal


	// write block size
	sendStream.device()->seek(0);
	sendStream << quint32(block.size() - sizeof(blockSize) - sizeof(marker));
	sendStream << marker << countBids << countAsks;

	client->write(block);

    //while (!client->waitForBytesWritten(500))
    //    ;
}



}
