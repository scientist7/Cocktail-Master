#include "Cocktail.h"

//--define static members
double Cocktail::ozincrements=0.25;
double Cocktail::tspperoz=6;
double Cocktail::solprecision=0.001;

//--Constructors
Cocktail::Cocktail(const std::vector<Ingredient> &list, 
				   const std::vector<Ingredient> &backup) {
	
	//--fill elements of cocktail
	for(auto el : list)
		elements.push_back(std::make_tuple(el,0,-1));

	//--fill reserve ingredients 
	for(auto el : backup)
		reserves.push_back(el);

}

void Cocktail::balance_drink() {
	//--group similar ingredients together
	//--and find number of unique ingredients
	eindex col=this->classify_ingredients();
	
	//--sort by group number
	sort(this->elements.begin(), this->elements.end(),
		[] (const element &lhs, const element &rhs)
	       {return std::get<2>(lhs) < std::get<2>(rhs);});

	//--compute overall flavor vector for each group
	//--for a group of collinear ingredients, the overall flavor
	//--vector is the weighted sum, where the weight is alpha/magnitude,
	//--alpha=1/((1/m1)+(1/m2)+...)

	std::vector<Eigen::Vector3d> grouped_ingredients(col);
	for(eindex i = 0; i < 3; ++i)
				grouped_ingredients[0](i) = 0;
	std::vector<double> group_norm(col,0);
	eindex gindex = 0;
	double invalpha = 0;
	for(eindex i = 0; i < elements.size(); ++i) {
		//--next group
		if(gindex != std::get<2>(elements[i])) {
			group_norm[gindex] = (1/invalpha);
			grouped_ingredients[gindex] *= group_norm[gindex];
			invalpha = 0;
			++gindex;
			if(gindex < col) {
				for(eindex i = 0; i < 3; ++i)
					grouped_ingredients[gindex](i) = 0;
			}
		}

		invalpha += 1/std::get<0>(elements[i]).get_flavor_magnitude();
		grouped_ingredients[gindex](0) += (std::get<0>(elements[i]).get_alcoholic_bite()
				                           /std::get<0>(elements[i]).get_flavor_magnitude());
	    grouped_ingredients[gindex](1) += (std::get<0>(elements[i]).get_sweetness()
				                           /std::get<0>(elements[i]).get_flavor_magnitude());
		grouped_ingredients[gindex](2) += (std::get<0>(elements[i]).get_sourness()
				                           /std::get<0>(elements[i]).get_flavor_magnitude());	
	}
	
	//--Finish last group
	group_norm[gindex] = (1/invalpha);
	grouped_ingredients[gindex] *= group_norm[gindex];
	
	//-- goal is to solve Ax=[1,1,1] for x

	//--x is a vector of coefficients for each ingredient
	//--A is a 3 x n matrix with each column representing a flavor vector
	const Eigen::Vector3d b(1,1,1);
	CMatrix A(3,col);
	Eigen::VectorXd x;
	//--loop to fill A
	for(eindex gindex = 0; gindex < col; ++gindex) {
		A.col(gindex) << grouped_ingredients[gindex](0),
				         grouped_ingredients[gindex](1),
					     grouped_ingredients[gindex](2);
	}
	
	
	try {
		//--fewer than 3 unique ingredients
		if(col<3) {
			Eigen::JacobiSVD<Eigen::MatrixXd> svd(3,col);
			x=svd.compute(A,Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
			double dwRelErr = ( A* x - b ).norm() / b.norm();
			if(dwRelErr>solprecision)
				throw no_solution("Overdetermined system with on solution");
		}
		//--more than 3 unique ingredients
		else if(col>3) 
			throw multiple_solutions("More than 3 unique ingredients for only 3 constraints");
	
		//--exactly 3 unique ingredients
		else {	
			Eigen::ColPivHouseholderQR<Eigen::Matrix3d> lu(A);
			x = lu.solve(b);
			if(x(0) < 0 || x(1) < 0 || x(2) < 0) 
					throw no_solution("Unphysical solution");
			double dwRelErr = ( A* x - b ).norm() / b.norm();
			if(dwRelErr>solprecision) throw no_solution("Inaccurate solution");
		}

		//--fill results into elements
		for(eindex i = 0; i < elements.size(); ++i) 
			std::get<1>(elements[i]) = x(std::get<2>(elements[i]))
					                   *(group_norm[std::get<2>(elements[i])]
									     /std::get<0>(elements[i]).get_flavor_magnitude());
										 
	} catch(const no_solution &e) {
		std::cerr << e.what() << std::endl;
        //--Try adding some standard ingredients to find a solution
		bool skip = false;
		for(eindex i = 0; i < reserves.size(); ++i) {
			skip = false;
			//--Check that it's not collinear with any existing ingredients
			for(eindex j = 0; j < elements.size(); ++j) {
				if(collinear(reserves[i], std::get<0>(elements[j]))) {
					skip=true;
					break;
				}
			}
			if(skip) continue;
			//--Here this reserve ingredient is tried
			
		}
	} catch(const multiple_solutions &e) {
		std::cerr << e.what() << std::endl;
		//--Condense ingredients with small opening angles together
	}
	catch(int e) {
		std::cout<<"Error code "<<e<<std::endl;
		return;
	}
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
	double oz=0,tsp=0;
	for(auto el : item.elements) {
		//--First convert raw oz amount to oz + tsp
		oz=std::get<1>(el);
		//--Report small amounts of strong flavors in tsp (e.g. sugar)
		if(oz <= 0.5 && std::get<0>(el).get_flavor_magnitude() >= 3) {
			tsp = oz;
			oz = 0;
		}
		else {
			tsp = fmod(oz,Cocktail::ozincrements);
			//--Check for precision problems
			if(fabs(tsp-Cocktail::ozincrements)<.001) tsp = 0;
			oz -= tsp;
		}
		tsp*=Cocktail::tspperoz;
		tsp=floor(tsp+0.5);
		os << std::left << std::setw(20) << std::get<0>(el) 
		   << " ";
		if(oz > 0) os << std::right << oz << " oz ";
		if(tsp > 0) os << tsp << " tsp";   
		os << std::endl;
	}
	return os;
}


