#pragma once
#include <iomanip>
#include <tuple>
#include <vector>
#include "Ingredient.h"
class Recipe
{
public:
	typedef std::tuple<Ingredient*, double> component;
	Recipe(const std::vector<Ingredient*> ins, 
		   const std::vector<double> ams);

	static double mlperoz;
	size_t getnumberofingredients() const { return components.size(); }
	Ingredient* getingredientat(size_t i) const { return std::get<0>(components[i]); }
	double getamountat(size_t i) const { return std::get<1>(components[i]); }

	//--Functions to calculate sum of flavor components
	size_t check_alcoholic_bite_sum(double &sum, double &effnummeas, size_t &index);
	size_t check_sweetness_sum(double &sum, double &effnummeas, size_t &index);
	size_t check_sourness_sum(double &sum, double &effnummeas, size_t &index);

    friend std::ostream &operator<<(std::ostream &os, const Recipe &recipe);

private: 
	std::vector<component> components;
	
};

