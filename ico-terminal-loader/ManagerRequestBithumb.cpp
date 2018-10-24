#include "ManagerRequestBithumb.h"
#include <QThread>
#include <QNetworkReply>

const int intervalCount = 5;

ManagerRequestBithumb::ManagerRequestBithumb(QObject *parent)
	: QObject(parent)
{
}


ManagerRequestBithumb::~ManagerRequestBithumb()
{
	manager->deleteLater();
	stop();
	timer->deleteLater();
	timerRequest->deleteLater();	
}

void ManagerRequestBithumb::run()
{
	manager      = new QNetworkAccessManager;
	connect(manager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply)
	{
		if (reply->error() != QNetworkReply::NoError)
		{
			auto date = QDateTime::currentDateTimeUtc();

			qDebug() << date.toString(Qt::ISODate) << " Bithumb (network): " << reply->errorString();

			reply->deleteLater();

			return;
		}

		const auto data = reply->readAll();
		reply->deleteLater();

		emit newData(data);
	});
	
	timer        = new QTimer;
	timerRequest = new QTimer;

	connect(timer, &QTimer::timeout, [this]()
	{
		for (const auto &r : requests)
		{
			waiting.push(r);
		}
	});
	
	const auto requestInterval = 1000 / (intervalCount-1);

	const int interval = (requests.size() * requestInterval) + ( 4 * requestInterval);
	
	timer->setInterval(interval);
	timer->start();

	connect(timerRequest, &QTimer::timeout, this, &ManagerRequestBithumb::request);

	timerRequest->setInterval(requestInterval);
	timerRequest->start();
}


void ManagerRequestBithumb::stop()
{
	if (timer)
	{
		timer->stop();
	}

	if (timerRequest)
	{
		timerRequest->stop();
	}
}

void ManagerRequestBithumb::request()
{
	if (waiting.empty()) return;

	if (waiting.size() > requests.size() * 2)
	{
		for (const auto &r : requests) waiting.pop();
	}

	auto reply = manager->get(
		waiting.front()
	);

	waiting.pop();

	reply->waitForReadyRead(200);
}
