#pragma once
#include <iomanip>
#include <tuple>
#include <vector>
#include <algorithm>
#include "Ingredient.h"

class Cocktail
{
	typedef std::tuple<Ingredient, double, int> element;
	typedef std::vector<element>::size_type eindex;
public:
	//--Constructors
	Cocktail(const std::vector<Ingredient> &);
	
	friend std::ostream &operator<<(std::ostream &os, 
		                            const Cocktail &item);

	void balance_drink();
	
private:
	std::vector<element> elements; 
	void classify_ingredients();
};

