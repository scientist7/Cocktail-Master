#include "Cocktail.h"

//--Constructors
Cocktail::Cocktail(const std::vector<Ingredient> &list) {
	//--fill elements of cocktail
	for(auto el : list)
		elements.push_back(std::make_tuple(el,0));
}

void Cocktail::balance_drink() {
	return;
}

//--overloaded operators
std::ostream &operator<<(std::ostream &os, const Cocktail &item) {
	for(auto el : item.elements) {
		os << std::left << std::setw(20) << std::get<0>(el) 
		   << " " << std::right << std::get<1>(el) << " oz"
		   << std::right << std::endl;
	}
	return os;
}


