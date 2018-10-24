#pragma once

#include <QObject>
#include <QTimer>
#include <QString>
#include <QNetworkReply>
#include <vector>

#include "DepthOrder.h"

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
QT_END_NAMESPACE

using namespace Entity;

namespace TerminalServer
{
class StorageOrderDepth;
}

namespace Loader
{

enum DepthOrderType : quint8
{
	Binance,
	Bitfinex,
	bitFlyer,
	Bithumb_api,
	CexIo,
	Exmo,
	Indodax,
	OKCoin,
	Huobi,
	Upbit,
	Wex
};

class AbstractLoader : public QObject
{
	Q_OBJECT

public:
	AbstractLoader(QObject *parent = nullptr);
	~AbstractLoader();

	void setInterval(uint);
	void setLimit(uint);
	void setForAllRateLimit(bool enabled = true);

	void connectStorage(TerminalServer::StorageOrderDepth *storage = nullptr);

	void run();

	static AbstractLoader * Generate(DepthOrderType type, QObject *parent = nullptr);

	public slots:
	virtual void loaded(QNetworkReply *) = 0;
	virtual void load() = 0;

signals:
	// index: [exchange]_[pair_currency_1][pair_curryncy_2]; Exmpl: wex_btcusd
	void dataRecieved(const DepthOrder&, const QString index);

protected:
	QTimer timer;

	QNetworkAccessManager *manager = nullptr;

	QString exchange = "";
	QString urlStr = "";
	uint    limit = 5000;

	std::vector<QString> pairs; // pair; frequency

	virtual void initPair() = 0; //TODO create loader pair from api exchange

	bool forAllRateLimit = false;
	uint rateLimit     = 600; 
	uint rateLimitTime = 1000 * 60 * 10; // 10 min

};


}