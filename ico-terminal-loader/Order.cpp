#include "Order.h"

namespace Entity
{
	Order::Order(double p, double a) : price(p), amount(a){}

	double Order::getPrice() const
	{
		return price;
	}

	double Order::getAmount() const
	{
		return amount;
	}

	void Order::setPrice(double value)
	{
		price = value;
	}

	void Order::setAmount(double value)
	{
		amount = value;
	}

	bool operator==(const Order & lhs, const Order & rhs)
	{
		return (lhs.getPrice() == rhs.getPrice() && 
			lhs.getAmount() == rhs.getAmount());
	}

	bool operator!=(const Order & lhs, const Order & rhs)
	{
		return !(lhs == rhs);
	}
	bool operator<(const Order & lhs, const Order & rhs)
	{
		return lhs.getPrice() < rhs.getPrice();
	}
	bool operator>(const Order & lhs, const Order & rhs)
	{
		return lhs.getPrice() > rhs.getPrice();
	}
}