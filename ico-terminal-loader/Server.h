#pragma once

#include <QObject>
#include <QByteArray>
#include <QVector>
#include <QMutex>
#include <QString>

#include "MainLoader.h"
#include "StorageOrderDepth.h"

QT_BEGIN_NAMESPACE
class QTcpServer;
class QTcpSocket;
QT_END_NAMESPACE

namespace TerminalServer
{

const quint16 defaultPort = 54769;

class Server : public QObject
{
	Q_OBJECT
public:
	explicit Server(QObject *parent = nullptr);
	~Server();

	void start(quint16 port = defaultPort);

signals:
	void closed();

private slots:
	
	void newConnection();
	void readData();
	void socketDisconnected();

	void sendDepthOrder(const Entity::DepthOrder*, QTcpSocket *, QString index, uint limmit = 0);

private:
	QTcpServer *server = nullptr;
	QVector<QTcpSocket *> clients;

	Loader::MainLoader loader;
	StorageOrderDepth storage;
};

}