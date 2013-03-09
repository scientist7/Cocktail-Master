#pragma once
#include <iomanip>
#include <tuple>
#include <vector>
#include "Ingredient.h"
class Recipe
{
public:
	typedef std::tuple<Ingredient, double> component;
	Recipe(std::vector<component> cps) : components(cps) {};

    friend std::ostream &operator<<(std::ostream &os, const Recipe &recipe);

private: 
	std::vector<component> components;
	
};

