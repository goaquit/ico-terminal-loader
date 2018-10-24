#include "Bithumb.h"
#include "json.hpp"
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>
#include "ManagerRequestBithumb.h"
#include <QThread>

using json = nlohmann::json;


namespace Loader
{


Bithumb::Bithumb(QObject *parent) : AbstractLoader(parent)
{
	urlStr = "https://api.bithumb.com/public/orderbook/";
	exchange = "Bithumb";

	forAllRateLimit = true;
	rateLimit = 100;
	rateLimitTime = 20000;
}


void Loader::Bithumb::load()
{
	// unused
}

void Loader::Bithumb::initPair()
{
	pairs.push_back("BTC");
	pairs.push_back("ETH");
	pairs.push_back("DASH");
	pairs.push_back("LTC");
	pairs.push_back("ETC");
	pairs.push_back("XRP");
	pairs.push_back("BCH");
	pairs.push_back("XMR");
	pairs.push_back("ZEC");
	pairs.push_back("QTUM");
	pairs.push_back("BTG");
	pairs.push_back("EOS");
	pairs.push_back("ICX");
	pairs.push_back("VEN");
	pairs.push_back("TRX");
	pairs.push_back("ELF");
	pairs.push_back("MITH");
	pairs.push_back("MCO");
	pairs.push_back("OMG");
	pairs.push_back("KNC");
	pairs.push_back("GNT");
	pairs.push_back("HSR");
	pairs.push_back("ZIL");
	pairs.push_back("ETHOS");
	pairs.push_back("PAY");
	pairs.push_back("WAX");
	pairs.push_back("POWR");
	pairs.push_back("LRC");
	pairs.push_back("GTO");
	pairs.push_back("STEEM");
	pairs.push_back("STRAT");
	pairs.push_back("ZRX");
	pairs.push_back("REP");
	pairs.push_back("AE");
	pairs.push_back("XEM");
	pairs.push_back("SNT");
	pairs.push_back("ADA");

	initRequest();
}

void Bithumb::initRequest()
{
	std::vector<QNetworkRequest> requests;

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
		QUrl url(urlStr + pair);
		url.setQuery("count=" + QString::number(limit));

		request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
		request.setUrl(url);

		requests.push_back(request);
	}

	auto managerRequest = new ManagerRequestBithumb;
	managerRequest->setRequests(requests);

	auto managerThread = new QThread;
	managerRequest->moveToThread(managerThread);

	connect(managerThread, &QThread::started, managerRequest, &ManagerRequestBithumb::run);
	connect(this, &QObject::destroyed, [managerRequest, managerThread]()
	{
		managerRequest->stop();
		managerThread->quit();
	});
	connect(managerRequest, &ManagerRequestBithumb::newData, this, &Bithumb::processData);

	managerThread->start();
}

void Bithumb::processData(const QByteArray & inputData)
{
	const std::string data(inputData.constData(), inputData.length());

	try
	{
		auto jsonData = json::parse(data.begin(), data.end());

		if (jsonData["status"].is_number() && jsonData["status"].get<uint>() > 0)
		{
			qDebug() << exchange << " has error";
		}

		for (const auto &pair : pairs)
		{
			DepthOrder depthOrder;

			const auto key = pair.toStdString();

			if (key.compare(jsonData["data"]["order_currency"].get<std::string>())) continue;

			for (auto el = jsonData["data"]["bids"].begin(); el != jsonData["data"]["bids"].end(); ++el)
			{
				const double price = std::stod((*el)["price"].get<std::string>());
				const double quantity = std::stod((*el)["quantity"].get<std::string>());

				const Order order(price, quantity);

				depthOrder.addBid(order);
			}

			for (auto el = jsonData["data"]["asks"].begin(); el != jsonData["data"]["asks"].end(); ++el)
			{
				const double price = std::stod((*el)["price"].get<std::string>());
				const double quantity = std::stod((*el)["quantity"].get<std::string>());

				const Order order(price, quantity);

				depthOrder.addAsk(order);
			}

			auto pairIndex = pair + QString::fromStdString(jsonData["data"]["payment_currency"].get<std::string>());
			const QString index = exchange.toLower() + "_" + pairIndex.toLower();

			emit dataRecieved(depthOrder, index);
		}

	}
	catch (nlohmann::detail::exception e)
	{
		auto date = QDateTime::currentDateTimeUtc();

		qDebug() << date.toString(Qt::ISODate) << exchange << " (json): " << e.what();
	}
}

void Loader::Bithumb::loaded(QNetworkReply *reply)
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

		if (jsonData["status"].is_number() && jsonData["status"].get<uint>() > 0)
		{
			qDebug() << exchange << " has error";
		}

		for (const auto &pair : pairs)
		{
			DepthOrder depthOrder;

			const auto key = pair.toStdString();

			if (key.compare(jsonData["data"]["order_currency"].get<std::string>())) continue;

			for (auto el = jsonData["data"]["bids"].begin(); el != jsonData["data"]["bids"].end(); ++el)
			{
				const double price    = std::stod( (*el)["price"].get<std::string>());
				const double quantity = std::stod((*el)["quantity"].get<std::string>());

				const Order order(price, quantity);

				depthOrder.addBid(order);
			}

			for (auto el = jsonData["data"]["asks"].begin(); el != jsonData["data"]["asks"].end(); ++el)
			{
				const double price    = std::stod((*el)["price"].get<std::string>());
				const double quantity = std::stod((*el)["quantity"].get<std::string>());

				const Order order(price, quantity);

				depthOrder.addAsk(order);
			}

			auto pairIndex = pair + QString::fromStdString(jsonData["data"]["payment_currency"].get<std::string>());
			const QString index = exchange.toLower() + "_" + pairIndex.toLower();

			emit dataRecieved(depthOrder, index);
		}

	}
	catch (nlohmann::detail::exception e)
	{
		auto date = QDateTime::currentDateTimeUtc();

		qDebug() << date.toString(Qt::ISODate) << exchange <<" (json): " << e.what();
	}
}


}