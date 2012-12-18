#pragma once
#include <iomanip>
#include <tuple>
#include <vector>
#include <algorithm>
#include "Ingredient.h"
#include <Eigen/Eigen>
#include "no_solution.h"
#include "multiple_solutions.h"


class Cocktail
{
	typedef std::tuple<Ingredient, double, int> element;
	typedef std::vector<element>::size_type eindex;
	typedef Eigen::Matrix<double, 3, Eigen::Dynamic> CMatrix;
public:
	//--Constructors
	Cocktail(const std::vector<Ingredient> &, 
		     const std::vector<Ingredient> &);
	
	friend std::ostream &operator<<(std::ostream &os, 
		                            const Cocktail &item);

	void balance_drink();
	
    //--Members controlling output
	static double ozincrements;
	static double tspperoz;

	//--Algorithm parameters
	static double solprecision;

private:
	std::vector<element> elements; 
	int classify_ingredients();
	void give_up();
	std::vector<Ingredient> reserves;
};

