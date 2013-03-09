#include "Recipe.h"

std::ostream &operator<<(std::ostream &os, const Recipe &recipe) {
	
	for(auto component : recipe.components) {
		os << std::left << std::setw(35) << std::get<0>(component) 
		   << " " << std::right << std::get<1>(component) << std::endl;
	}
	return os;
}
