#pragma once
#include "Person.h"

// INHERITANCE: Customer inherits from Person
class Customer : public Person
{
public:
	Customer(const std::string& n, const std::string& p, const std::string& e)
		: Person(n, p, e) {
	}

	// POLYMORPHISM: Customer's own version of getDetails()
	std::string getDetails() const override
	{
		return "Customer: " + getName() + " | Phone: " + getPhone();
	}
};