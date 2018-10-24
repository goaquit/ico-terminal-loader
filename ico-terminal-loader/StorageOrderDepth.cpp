#include "StorageOrderDepth.h"
#include <stdexcept>

namespace TerminalServer
{


StorageOrderDepth::StorageOrderDepth(QObject *parent)
	: QObject(parent)
{
}

StorageOrderDepth::~StorageOrderDepth()
{
	storage.clear();
}

const DepthOrder * StorageOrderDepth::getDepthOrder(const QString & index) const
{	
	const DepthOrder * result = nullptr;
	

	auto itr = storage.find(index);
	if (itr != storage.end())
	{
		result = &(itr->second);
	}

	return result;
}

void StorageOrderDepth::dataRecieved(const DepthOrder &depthOrder, const QString index)
{
	storage[index] = depthOrder;
}


}