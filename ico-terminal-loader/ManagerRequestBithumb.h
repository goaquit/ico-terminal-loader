#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <queue>
#include <QTimer>
#include <QByteArray>
#include <QNetworkRequest>

class ManagerRequestBithumb : public QObject
{
	Q_OBJECT

public:
	ManagerRequestBithumb(QObject *parent = nullptr);
	~ManagerRequestBithumb();

	inline void setRequests(const std::vector<QNetworkRequest> &r) {requests = r;}

public slots:
	void run();
	void stop();
	void request();

signals:
	void newData(const QByteArray &data);

private:
	std::vector<QNetworkRequest> requests;
	std::queue<QNetworkRequest> waiting;

	QNetworkAccessManager *manager	    = nullptr;
	QTimer			      *timer	    = nullptr;
	QTimer				  *timerRequest = nullptr;
};
