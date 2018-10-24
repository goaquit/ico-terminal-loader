#pragma once

#include "AbstractLoader.h"
#include <QByteArray>
#include <vector>

namespace Loader
{


class Bithumb : public AbstractLoader
{
public:
	Bithumb(QObject *parent = nullptr);

	virtual void load() override;
	virtual void loaded(QNetworkReply *);

protected:
	virtual void initPair() override;

private:
	void initRequest();

private slots:
	void processData(const QByteArray &data);

};


}