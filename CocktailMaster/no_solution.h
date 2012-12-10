#pragma once
#include <stdexcept>
class no_solution :
	public std::runtime_error
{
public:
	explicit no_solution(const std::string &s) :
		std::runtime_error(s) {}
	
};

