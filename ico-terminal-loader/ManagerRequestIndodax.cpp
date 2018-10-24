#include "ManagerRequestIndodax.h"
#include <QThread>
#include <QNetworkReply>

const int requestInterval = 600; //msec

ManagerRequestIndodax::ManagerRequestIndodax(QObject *parent)
	: QObject(parent)
{
}

ManagerRequestIndodax::~ManagerRequestIndodax()
{
	manager->deleteLater();
	stop();
	timer->deleteLater();
	timerRequest->deleteLater();
}

void ManagerRequestIndodax::run()
{
	manager = new QNetworkAccessManager;
	connect(manager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply)
	{
		if (reply->error() != QNetworkReply::NoError)
		{
			auto date = QDateTime::currentDateTimeUtc();

			qDebug() << date.toString(Qt::ISODate) << " Indodax (network): " << reply->errorString();

			reply->deleteLater();

			return;
		}

		const auto data = reply->readAll();
		const auto url  = reply->url();
		reply->deleteLater();

		emit newData(data, url.toString());
	});

	timer = new QTimer;
	timerRequest = new QTimer;

	connect(timer, &QTimer::timeout, [this]()
	{
		for (const auto &r : requests)
		{
			waiting.push(r);
		}
	});

	const int interval = requestInterval * requests.size() + (requestInterval * 3);

	timer->setInterval(interval);
	timer->start();

	connect(timerRequest, &QTimer::timeout, this, &ManagerRequestIndodax::request);

	timerRequest->setInterval(requestInterval);
	timerRequest->start();
}

void ManagerRequestIndodax::stop()
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

void ManagerRequestIndodax::request()
{
	if (waiting.empty()) return;

	if (waiting.size() > requests.size() * 2)
	{
		for (const auto &r : requests)
		{
			Q_UNUSED(r);
			waiting.pop();
		}
	}

	auto reply = manager->get(
		waiting.front()
	);

	waiting.pop();

	reply->waitForReadyRead(requestInterval);
}
