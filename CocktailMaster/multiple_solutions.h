#pragma once
#include <stdexcept>
class multiple_solutions :
	public std::logic_error
{
public:
	explicit multiple_solutions(const std::string &s) :
		std::logic_error(s) {}
};

