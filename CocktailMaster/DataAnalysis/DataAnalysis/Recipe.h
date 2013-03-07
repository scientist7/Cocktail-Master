#pragma once
#include <tuple>
#include <vector>
#include "Ingredient.h"
class Recipe
{
	typedef std::tuple<Ingredient, double> component;

public:
	Recipe(std::vector<component> cps) : components(cps) {};

private: 
	std::vector<component> components;
	
};

