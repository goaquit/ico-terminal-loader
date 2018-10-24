#pragma once
#include "AbstractLoader.h"

namespace Loader
{


class LoaderIndodax : public AbstractLoader
{
public:
	LoaderIndodax(QObject *parent = nullptr);

	virtual void load() override;
	virtual void loaded(QNetworkReply *);

protected:
	virtual void initPair() override;

	void initRequest();

private slots:
	void processData(const QByteArray &data, const QString &query);
};

}