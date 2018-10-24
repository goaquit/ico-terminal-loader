#pragma once

namespace Entity
{

	class Order
	{
	public:

		Order() = default;
		Order(double price, double amount);
		
		double    getPrice()  const;
		double    getAmount() const;

		void setPrice(double);
		void setAmount(double);

	private:
		double price  = 0.0;
		double amount = 0.0;
	};

	bool operator == (const Order& lhs, const Order& rhs);
	bool operator != (const Order& lhs, const Order& rhs);
	bool operator <  (const Order& lhs, const Order& rhs);
	bool operator >  (const Order& lhs, const Order& rhs);
}