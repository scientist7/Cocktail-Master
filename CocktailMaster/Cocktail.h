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

//--Types
typedef Eigen::Matrix<double, 3, Eigen::Dynamic> CMatrix;
typedef std::tuple<Ingredient, double, int> element;
typedef std::vector<element>::size_type eindex;
typedef std::multimap<double, Eigen::VectorXd> candsolutions;

//--Non-member functions
bool solve_overdetermined(const CMatrix &, Eigen::VectorXd &, 
					      bool throwflag = false);
bool solve_squarematrix(const CMatrix &, Eigen::VectorXd &, 
					    bool throwflag = false);
bool check_nosolutions(const CMatrix &, const Eigen::Vector3d &);
bool find_optimum(Eigen::VectorXd &, const CMatrix &, const Eigen::Vector3d &,
		          bool printinfo);
void search(eindex, const CMatrix &, Eigen::VectorXd &, const Eigen::Vector3d &,
		    bool &, Eigen::VectorXd &, candsolutions &);
double figure_of_merit(const Eigen::VectorXd &x, const CMatrix &A);
double round_to_multiple(const double, const double);
inline bool compare_group(const element &lhs, const element &rhs) 
                         {return std::get<2>(lhs) < std::get<2>(rhs);};


class Cocktail
{
	
public:
	//--Constructors
	Cocktail(const std::vector<Ingredient> &, 
		     const std::vector<Ingredient> &,
			 const bool pinfo = false);
	
	friend std::ostream &operator<<(std::ostream &os, 
		                            const Cocktail &item);

	void balance_drink();
	void scale_recipe();
	void output_webpage();
	
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
	
	//--Data members
	std::vector<element> elements;
	std::vector<Ingredient> reserves;
	bool printinfo;
};




