#include "Recipe.h"

double Recipe::mlperoz = 30;

Recipe::Recipe(const std::vector<Ingredient*> ins, 
		   const std::vector<double> ams) {
	for(size_t i = 0; i < ins.size(); ++i) {
		components.push_back(std::make_tuple(ins[i],ams[i]/mlperoz));
	}
}

std::ostream &operator<<(std::ostream &os, const Recipe &recipe) {
	
	for(auto component : recipe.components) {
		os << std::left << std::setw(35) << *std::get<0>(component) 
		   << " " << std::right << std::get<1>(component) << std::endl;
	}
	return os;
}
