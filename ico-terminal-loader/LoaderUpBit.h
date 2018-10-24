#pragma once

#include "AbstractLoader.h"

namespace Loader
{

class LoaderUpBit : public AbstractLoader
{
	Q_OBJECT

public:
	LoaderUpBit(QObject *parent = nullptr);

	virtual void load() override;
	virtual void loaded(QNetworkReply *);

protected:
	virtual void initPair() override;

};

}