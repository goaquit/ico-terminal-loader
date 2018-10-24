#include "AbstractLoader.h"
#include "StorageOrderDepth.h"
#include <QNetworkAccessManager>

// loader
#include "LoaderCexIo.h"
#include "LoaderWex.h"
#include "LoaderExmo.h"
#include "Bithumb.h"
#include "LoaderBitfinex.h"
#include "LoaderBinance.h"
#include "LoaderHuobi.h"
#include "LoaderIndodax.h"
#include "LoaderOkCoin.h"
#include "LoaderBitFlyer.h"
#include "LoaderUpBit.h"

static const uint defaultInterval = 10000; // Default timer interval: 3 sec

inline uint rateInterval(uint limit, uint time, uint count)
{
	return (time * count) / limit;
}

namespace Loader
{




AbstractLoader::AbstractLoader(QObject *parent)
	: QObject(parent), manager(new QNetworkAccessManager(parent))
{
	setInterval(defaultInterval);

	QObject::connect(manager,         &QNetworkAccessManager::finished, this, &AbstractLoader::loaded);
	QObject::connect(&timer,          &QTimer::timeout,                 this, &AbstractLoader::load);
}

AbstractLoader::~AbstractLoader()
{
	timer.stop();
	manager->deleteLater();
}

void AbstractLoader::setInterval(uint interval)
{
	timer.setInterval(interval);
}

void AbstractLoader::setLimit(uint value)
{
	limit = value;
}

void AbstractLoader::setForAllRateLimit(bool enabled)
{
	forAllRateLimit = enabled;
}

void AbstractLoader::connectStorage(TerminalServer::StorageOrderDepth *storage)
{
	if (!storage) return;

	QObject::connect(this, &AbstractLoader::dataRecieved, storage, &TerminalServer::StorageOrderDepth::dataRecieved);
}

void AbstractLoader::run()
{
	initPair();

	if (forAllRateLimit)
	{
		setInterval(rateInterval(rateLimit, rateLimitTime, static_cast<uint>(pairs.size())));
	}
	

	timer.start();
}


AbstractLoader * AbstractLoader::Generate(DepthOrderType type, QObject * parent)
{
	AbstractLoader * result = nullptr;

	switch (type)
	{
	case Loader::Binance:
		result = new Loader::LoaderBinance(parent);
		break;
	case Loader::Bitfinex:
		result = new Loader::LoaderBitfinex(parent);
		break;
	case Loader::bitFlyer:
		result = new LoaderBitFlyer(parent);
		break;
	case Loader::Bithumb_api:
		result = new Loader::Bithumb(parent);
		break;
	case Loader::CexIo: 

		result = new LoaderCexIo(parent);
		result->setForAllRateLimit();
	break;
	case Loader::Exmo:
		result = new LoaderExmo(parent);
        result->setForAllRateLimit();
		break;
	case Loader::Indodax:
		result = new LoaderIndodax(parent);
		break;
	case Loader::OKCoin:
		result = new LoaderOkCoin(parent);
		break;
	case Loader::Huobi:
		result = new LoaderHuobi(parent);
		break;
	case Loader::Wex:
		result = new LoaderWex(parent);
		break;
	case Loader::Upbit:
		result = new LoaderUpBit(parent);
		break;
	}


	return result;
}



}
