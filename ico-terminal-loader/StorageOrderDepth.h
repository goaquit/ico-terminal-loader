#pragma once

#include <QObject>
#include "DepthOrder.h"
#include <map>
#include <QString>
#include <QMutex>

using namespace Entity;

namespace TerminalServer
{

class StorageOrderDepth : public QObject
{
	Q_OBJECT

public:
	StorageOrderDepth(QObject *parent = nullptr);
	~StorageOrderDepth();

	const DepthOrder * getDepthOrder(const QString & index) const;

public slots:
	void dataRecieved(const DepthOrder&, const QString index);

private:
	std::map<QString, DepthOrder> storage;
};


}