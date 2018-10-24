#pragma once

#include <QObject>

#include <memory>
#include <vector>

#include "StorageOrderDepth.h"

using namespace TerminalServer;

namespace Loader
{

class AbstractLoader;

class MainLoader : public QObject
{
	Q_OBJECT

public:
	MainLoader(QObject *parent = nullptr);
	~MainLoader();

	void start();
	void setStorageDepth(StorageOrderDepth *);

private:
	std::vector<std::unique_ptr<AbstractLoader> > loaders;
	StorageOrderDepth *storageOrderDepth = nullptr;
};


}