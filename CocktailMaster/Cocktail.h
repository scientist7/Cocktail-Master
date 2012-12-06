#pragma once
#include <iomanip>
#include <tuple>
#include <vector>
#include <algorithm>
#include "Ingredient.h"
#include <Eigen/Eigen>


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
	
    //--Members controlling output
	static double ozincrements;
	static double tspperoz;

private:
	std::vector<element> elements; 
	int classify_ingredients();
};

