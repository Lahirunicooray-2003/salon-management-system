#pragma once
#include <string>
#include <stdexcept>

// ABSTRACTION: abstract class, objects cannot be created from it
class Person
{
private:   // ENCAPSULATION: private data
	int id;
	std::string name;
	std::string phone;
	std::string email;

public:
	Person(const std::string& name, const std::string& phone, const std::string& email)
		: id(0)
	{
		setName(name);
		setPhone(phone);
		setEmail(email);
	}
	virtual ~Person() {}

	int getId() const { return id; }
	void setId(int v) { id = v; }

	std::string getName() const { return name; }
	void setName(const std::string& v)
	{
		if (v.empty()) throw std::invalid_argument("Name cannot be empty.");
		name = v;
	}

	std::string getPhone() const { return phone; }
	void setPhone(const std::string& v)
	{
		if (v.length() != 10) throw std::invalid_argument("Phone number must have exactly 10 digits.");
		phone = v;
	}

	std::string getEmail() const { return email; }
	void setEmail(const std::string& v) { email = v; }

	// Pure virtual function: child classes must implement it
	virtual std::string getDetails() const = 0;
};