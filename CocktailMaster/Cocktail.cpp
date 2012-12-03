#include "Cocktail.h"

//--Constructors
Cocktail::Cocktail(const std::vector<Ingredient> &list) {
	//--fill elements of cocktail
	for(auto el : list)
		elements.push_back(std::make_tuple(el,0,-1));
}

void Cocktail::balance_drink() {
	//--group similar ingredients together
	//--and find number of unique ingredients
	int col=this->classify_ingredients();
	
	//--sort by group number
	sort(this->elements.begin(), this->elements.end(),
		[] (const element &lhs, const element &rhs)
	       {return std::get<2>(lhs) < std::get<2>(rhs);});

	//-- goal is to solve Ax=[1,1,1] for x
	//--x is a vector of coefficients for each ingredient
	//--A is a 3 x n matrix with each column representing a flavor vector
	const Eigen::Vector3d b(1,1,1);
	Eigen::Matrix<double, 3, Eigen::Dynamic> A(3,col);
	//--loop to fill A
	for(auto el : elements) {
		A.col(std::get<2>(el)) << std::get<0>(el).get_alcoholic_bite(),
		                          std::get<0>(el).get_sweetness(),
								  std::get<0>(el).get_sourness();
	}
	Eigen::VectorXd x = A.colPivHouseholderQr().solve(b);
	//--fill results into elements
	for(eindex i = 0; i < elements.size(); ++i) 
		std::get<1>(elements[i]) = x(std::get<2>(elements[i]));
	return;
}

int Cocktail::classify_ingredients() {
	//--Any two vectors in flavor space are classified
	//--as equivalent if they are collinear
	eindex group_number=0;
	for(eindex i1 = 0; i1 < elements.size() - 1; ++i1) {
		//--If primary element hasn't been assigned a group index, assign one
		if(std::get<2>(elements[i1]) == -1) {
			std::get<2>(elements[i1]) = group_number;
			++group_number;
		}
		for(eindex i2 = i1 + 1; i2 < elements.size(); ++i2) {
			//--skip if already assigned or if not collinear with primary element
			if(std::get<2>(elements[i2]) != -1 
			   || !collinear(std::get<0>(elements[i1]),
			   std::get<0>(elements[i2]))) continue;
			//--otherwise equate group numbers
			std::get<2>(elements[i2]) = std::get<2>(elements[i1]);
		}
	}
	//--Make sure last ingredient is assigned a group number
	if(std::get<2>(elements[elements.size() - 1]) == -1 ) {
		std::get<2>(elements[elements.size() - 1]) = group_number;
		++group_number;
	}
	//--return number of unique ingredients (not related by collinearity)
	return group_number;
}

//--overloaded operators
std::ostream &operator<<(std::ostream &os, const Cocktail &item) {
	for(auto el : item.elements) {
		os << std::left << std::setw(20) << std::get<0>(el) 
		   << " " << std::right << std::get<1>(el) << " oz"
		   << std::endl;
	}
	return os;
}


