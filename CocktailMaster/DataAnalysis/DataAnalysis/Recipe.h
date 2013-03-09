#pragma once
#include <iomanip>
#include <tuple>
#include <vector>
#include "Ingredient.h"
class Recipe
{
public:
	typedef std::tuple<Ingredient, double> component;
	Recipe(const std::vector<Ingredient> ins, 
		   const std::vector<double> ams);

    friend std::ostream &operator<<(std::ostream &os, const Recipe &recipe);

private: 
	std::vector<component> components;
	
};

