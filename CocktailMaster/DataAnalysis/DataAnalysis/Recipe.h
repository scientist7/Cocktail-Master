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

    friend std::ostream &operator<<(std::ostream &os, const Recipe &recipe);

private: 
	std::vector<component> components;
	
};

