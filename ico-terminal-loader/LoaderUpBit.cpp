#include "LoaderUpBit.h"
#include "json.hpp"
#include <QNetworkRequest>
#include <QDebug>

using json = nlohmann::json;

namespace Loader
{


LoaderUpBit::LoaderUpBit(QObject *parent)
	: AbstractLoader(parent)
{
	urlStr = "https://api.upbit.com/v1/orderbook/";
	exchange = "UPbit";
}

void LoaderUpBit::load()
{
	if (!pairs.size()) return;

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
		QUrl url(urlStr);
		url.setQuery("markets=" + pair);

		request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
		request.setUrl(url);

		manager->get(request);
	}
}

void LoaderUpBit::initPair()
{
	pairs.push_back("KRW-EOS");
	pairs.push_back("KRW-BTC");
	pairs.push_back("KRW-ADA");
	pairs.push_back("KRW-TRX");
	pairs.push_back("KRW-ZRX");
	pairs.push_back("KRW-ETH");
	pairs.push_back("KRW-XRP");
	pairs.push_back("KRW-POWR");
	pairs.push_back("KRW-ETC");
	pairs.push_back("KRW-GTO");
	pairs.push_back("KRW-BCH");
	pairs.push_back("KRW-ZIL");
	pairs.push_back("KRW-SNT");
	pairs.push_back("KRW-QTUM");
	pairs.push_back("KRW-ICX");
	pairs.push_back("KRW-BTG");
	pairs.push_back("KRW-GNT");
	pairs.push_back("KRW-REP");
	pairs.push_back("KRW-MCO");
	pairs.push_back("KRW-OMG");
	pairs.push_back("KRW-LTC");
	pairs.push_back("KRW-XEM");
	pairs.push_back("KRW-ZEC");
	pairs.push_back("KRW-DASH");
	pairs.push_back("KRW-STRAT");
	pairs.push_back("KRW-XMR");


	pairs.push_back("USDT-BTC");
	pairs.push_back("USDT-ETH");
	pairs.push_back("USDT-DASH");
	pairs.push_back("USDT-LTC");
	pairs.push_back("USDT-ETC");
	pairs.push_back("USDT-XRP");
	pairs.push_back("USDT-BCH");
	pairs.push_back("USDT-XMR");
	pairs.push_back("USDT-ZEC");
}

void LoaderUpBit::loaded(QNetworkReply *reply)
{
	if (reply->error() != QNetworkReply::NoError)
	{
		auto date = QDateTime::currentDateTimeUtc();

		qDebug() << date.toString(Qt::ISODate) << exchange << " (network): " << reply->errorString();

		reply->deleteLater();

		return;
	}

	const auto url = reply->url();
	const auto byteData = reply->readAll();
	const std::string data(byteData.constData(), byteData.length());
	reply->deleteLater();

	try
	{
		auto jsonData = json::parse(data.begin(), data.end());

		if (jsonData.find("error") != jsonData.end())
		{
			const auto message = QString::fromStdString(
				jsonData["error"]["message"].get<std::string>()
			);

			qDebug() << QDateTime::currentDateTimeUtc().toString(Qt::ISODate) << exchange << " (error): " << message;
			return;
		}

		jsonData = jsonData.at(0);

		if (jsonData.find("market") == jsonData.end())
		{
			qDebug() << QDateTime::currentDateTimeUtc().toString(Qt::ISODate) << exchange << " (json): Market not found";
			return;
		}

		for (const auto &pair : pairs)
		{
			DepthOrder depthOrder;	

			const auto market = QString::fromStdString(jsonData["market"].get<std::string>());

			if (pair.compare(market, Qt::CaseInsensitive)) continue;

			for (auto el = jsonData["orderbook_units"].begin(); el != jsonData["orderbook_units"].end(); ++el)
			{
				depthOrder.addBid(Order(
					(*el)["bid_price"].get<double>(),
					(*el)["bid_size"].get<double>()
				));

				depthOrder.addAsk(Order(
					(*el)["ask_price"].get<double>(),
					(*el)["ask_size"].get<double>()
				));
			}


			const auto asks = jsonData["total_ask_size"].get<double>();
			const auto bids = jsonData["total_bid_size"].get<double>();

			depthOrder.setQuantityAsks(asks);
			depthOrder.setQuantityBids(bids);

			QRegExp rxlen("(\\w+)-(\\w+)");
			int pos = rxlen.indexIn(pair);
			if (pos == -1) 
			{
				return;
			} 

			QString base  = rxlen.cap(1);
			QString quote = rxlen.cap(2);

			const QString index = exchange.toLower() + "_" + (quote + base).toLower();

			emit dataRecieved(depthOrder, index);
		}

	}
	catch (nlohmann::detail::exception e)
	{
		auto date = QDateTime::currentDateTimeUtc();

		qDebug() << date.toString(Qt::ISODate) << exchange << " (json): " << e.what();
	}
}

}