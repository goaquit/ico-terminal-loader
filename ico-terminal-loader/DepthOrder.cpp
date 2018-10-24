#include "DepthOrder.h"
#include <algorithm>

namespace Entity
{
void DepthOrder::addAsk(const Order &order)
{
	vectorAsks.push_back(order);
	qntAsks += order.getAmount();
}

void DepthOrder::addBid(const Order &order)
{
	vectorBids.push_back(order);
	qntBids += order.getAmount();
}

std::vector<Order> DepthOrder::asks() const
{
	return vectorAsks;
}

std::vector<Order> DepthOrder::bids() const
{
	return vectorBids;
}

double DepthOrder::quantityAsks() const
{
	return qntAsks;
}

double DepthOrder::quantityBids() const
{
	return qntBids;
}

void DepthOrder::setQuantityAsks(double value)
{
	qntAsks = value;
}

void DepthOrder::setQuantityBids(double value)
{
	qntBids = value;
}

void DepthOrder::sort()
{
	std::sort(vectorAsks.begin(), vectorAsks.end());
	std::sort(vectorBids.begin(), vectorBids.end());
}

}