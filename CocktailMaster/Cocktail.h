#pragma once
#include <iomanip>
#include <tuple>
#include <vector>
#include <map>
#include <algorithm>
#include "Ingredient.h"
//#include <Eigen>
#include <Eigen/Eigen>
#include "no_solution.h"
#include "multiple_solutions.h"

typedef Eigen::Matrix<double, 3, Eigen::Dynamic> CMatrix;

class Cocktail
{
	typedef std::tuple<Ingredient, double, int> element;
	typedef std::vector<element>::size_type eindex;
	typedef std::multimap<double, Eigen::VectorXd> candsolutions;

	double round_to_multiple(const double, const double);
	
public:
	//--Constructors
	Cocktail(const std::vector<Ingredient> &, 
		     const std::vector<Ingredient> &);
	
	friend std::ostream &operator<<(std::ostream &os, 
		                            const Cocktail &item);

	void balance_drink();
	void scale_recipe();
	
    //--Members controlling output
	static const double ozincrements;
	static const double mlincrements;
	static const double tspperoz;
	static const double mlperoz;

	//--Algorithm parameters
	static const double solprecision;
	static const double scaleprecision;
	
private:
	 
	bool add_ingredient(const CMatrix &, CMatrix &, Eigen::VectorXd &, 
		                const Eigen::Vector3d &, eindex, eindex j = 0);
	eindex classify_ingredients();
	void give_up();
	friend bool solve_overdetermined(const CMatrix &, Eigen::VectorXd &, 
		                            bool throwflag = false);
	friend bool solve_squarematrix(const CMatrix &, Eigen::VectorXd &, 
		                            bool throwflag = false);
	friend bool check_nosolutions(const CMatrix &, const Eigen::Vector3d &);
	friend bool find_optimum(Eigen::VectorXd &, const CMatrix &, const Eigen::Vector3d &);
	friend void search(eindex, const CMatrix &, Eigen::VectorXd &, const Eigen::Vector3d &,
		               bool &, Eigen::VectorXd &, candsolutions &);
	friend double figure_of_merit(const Eigen::VectorXd &x, const CMatrix &A);

	//--Data members
	std::vector<element> elements;
	std::vector<Ingredient> reserves;
};




