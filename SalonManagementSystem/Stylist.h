#pragma once
#include "Person.h"

// INHERITANCE: Stylist inherits from Person
class Stylist : public Person
{
private:
	std::string speciality;

public:
	Stylist(const std::string& n, const std::string& p, const std::string& e, const std::string& s)
		: Person(n, p, e), speciality(s) {
	}

	std::string getSpeciality() const { return speciality; }
	void setSpeciality(const std::string& s) { speciality = s; }

	// POLYMORPHISM: Stylist's own version of getDetails()
	std::string getDetails() const override
	{
		return "Stylist: " + getName() + " | Speciality: " + getSpeciality();
	}
};