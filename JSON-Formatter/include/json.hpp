#include <iostream>
#include <string>
#include <limits>
#include <cassert>
#include <utility>
#include <vector>
#include <fstream>

struct json_exception : public std::exception{
	std::string msg;
    explicit json_exception(std::string str) : msg(std::move(str)) {}
};

class json {

public:
	
	struct list_iterator;
	struct dictionary_iterator;
	struct const_list_iterator;
	struct const_dictionary_iterator;
	
	json();
	json(const json&);
	json(json&&) noexcept ;
	~json();
	
	json& operator=(json const&);
	json& operator=(json&&) noexcept ;

	[[nodiscard]] bool is_list() const;
	[[nodiscard]] bool is_dictionary() const;
	[[nodiscard]] bool is_string() const;
	[[nodiscard]] bool is_number() const;
	[[nodiscard]] bool is_bool() const;
	[[nodiscard]] bool is_null() const;

	json const& operator[](std::string const&) const;
	json& operator[](std::string const&);

	list_iterator begin_list();
	[[nodiscard]] const_list_iterator begin_list() const;
	list_iterator end_list();
	[[nodiscard]] const_list_iterator end_list() const;

	dictionary_iterator begin_dictionary();
	[[nodiscard]] const_dictionary_iterator begin_dictionary() const;
	dictionary_iterator end_dictionary();
	[[nodiscard]] const_dictionary_iterator end_dictionary() const;

	double& get_number();
	[[nodiscard]] double const& get_number() const;

	bool& get_bool();
	[[nodiscard]] bool const& get_bool() const;

	std::string& get_string();
	[[nodiscard]] std::string const& get_string() const;

	void set_string(const std::string&);
	void set_bool(bool);
	void set_number(double);
	void set_null();
	void set_list();
	void set_dictionary();
	void push_front(const json&);
	void push_back(const json&);
	void insert(const std::pair<std::string,json>&);

private:
		
	struct impl;
	impl* pimpl;
	
};

std::ostream& operator<<(std::ostream& lhs, json const& rhs);
std::istream& operator>>(std::istream& lhs, json& rhs);