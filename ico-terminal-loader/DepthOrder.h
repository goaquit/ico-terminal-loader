#pragma once

#include "Order.h"
#include <vector>

namespace Entity
{


class DepthOrder
{
public:
	void addAsk(const Order&);
	void addBid(const Order&);

	std::vector<Order> asks() const;
	std::vector<Order> bids() const;

	double quantityAsks() const;
	double quantityBids() const;

	void setQuantityAsks(double);
	void setQuantityBids(double);

	void sort();

private:

	std::vector<Order> vectorAsks;
	std::vector<Order> vectorBids;

	double qntAsks = 0;
	double qntBids = 0;
};


}