#include "MainLoader.h"
#include "AbstractLoader.h"

namespace Loader
{


using uniqLoader = std::unique_ptr<AbstractLoader>;


MainLoader::MainLoader(QObject *parent)
	: QObject(parent)
{
	loaders.push_back(uniqLoader( AbstractLoader::Generate(DepthOrderType::CexIo,       parent)) );
	loaders.push_back(uniqLoader( AbstractLoader::Generate(DepthOrderType::Exmo,        parent)) );
	loaders.push_back(uniqLoader( AbstractLoader::Generate(DepthOrderType::Wex,         parent)) );
	loaders.push_back(uniqLoader( AbstractLoader::Generate(DepthOrderType::Bithumb_api, parent)) );
	loaders.push_back(uniqLoader( AbstractLoader::Generate(DepthOrderType::Bitfinex,    parent)) );
	loaders.push_back(uniqLoader( AbstractLoader::Generate(DepthOrderType::Binance,     parent)) );
	loaders.push_back(uniqLoader( AbstractLoader::Generate(DepthOrderType::Huobi,       parent)) );
	loaders.push_back(uniqLoader( AbstractLoader::Generate(DepthOrderType::Indodax,     parent)) );
	loaders.push_back(uniqLoader( AbstractLoader::Generate(DepthOrderType::OKCoin,      parent)) );
	loaders.push_back(uniqLoader( AbstractLoader::Generate(DepthOrderType::bitFlyer,    parent)) );
	loaders.push_back(uniqLoader( AbstractLoader::Generate(DepthOrderType::Upbit,		parent)));
}

MainLoader::~MainLoader()
{
	loaders.clear();
}

void MainLoader::start()
{

	for (const auto &loader : loaders)
	{
		if (!loader.get()) continue;		
		loader->connectStorage(storageOrderDepth);
		loader->run();
	}

}

void MainLoader::setStorageDepth(StorageOrderDepth *storage)
{
	storageOrderDepth = storage;
}


}