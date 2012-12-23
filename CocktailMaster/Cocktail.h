#pragma once
#include <iomanip>
#include <tuple>
#include <vector>
#include <algorithm>
#include "Ingredient.h"
#include <Eigen/Eigen>
#include "no_solution.h"
#include "multiple_solutions.h"

typedef Eigen::Matrix<double, 3, Eigen::Dynamic> CMatrix;

class Cocktail
{
	typedef std::tuple<Ingredient, double, int> element;
	typedef std::vector<element>::size_type eindex;
	
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
	bool add_ingredient(const CMatrix &, CMatrix &, Eigen::VectorXd &, eindex, eindex j = 0);
	eindex classify_ingredients();
	void give_up();
	friend bool solve_overdetermined(const CMatrix &, Eigen::VectorXd &, 
		                            bool throwflag = false);
	friend bool solve_squarematrix(const CMatrix &, Eigen::VectorXd &, 
		                            bool throwflag = false);
	friend bool check_nosolutions(const CMatrix &, const Eigen::Vector3d &);
	friend bool find_optimum(Eigen::VectorXd &, const CMatrix &, const Eigen::Vector3d &);
	friend void search(eindex, const CMatrix &, Eigen::VectorXd &, const Eigen::Vector3d &,
		               bool &, double &, Eigen::VectorXd &);
	friend double figure_of_merit(const Eigen::VectorXd &x, const CMatrix &A);
	std::vector<Ingredient> reserves;
};




