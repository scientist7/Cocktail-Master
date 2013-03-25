#include "Cocktail.h"

//--define static members
double Cocktail::ozincrements=0.25;
double Cocktail::tspperoz=6;
double Cocktail::solprecision=0.001;
double Cocktail::mlperoz=30;

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
	Eigen::VectorXd x(col);
	//--loop to fill A
	for(eindex gindex = 0; gindex < col; ++gindex) {
		A.col(gindex) << grouped_ingredients[gindex](0),
				         grouped_ingredients[gindex](1),
					     grouped_ingredients[gindex](2);
	}
	
	
	try {
		//--fewer than 3 unique ingredients
		if(col<3) solve_overdetermined(A,x,true);
		//--more than 3 unique ingredients
		else if(col>3) {
			if(check_nosolutions(A,b)) throw no_solution("More than 3 ingredients, but no solution");
			throw multiple_solutions("More than 3 unique ingredients for only 3 constraints");
		}
		//--exactly 3 unique ingredients
		else solve_squarematrix(A,x,true);
										 
	} catch(const no_solution &e) {
		std::cerr << e.what() << std::endl;
		bool success = false;
        //--Try adding some standard ingredients to find a solution
		for(eindex i = 0; i < reserves.size(); ++i) {
			CMatrix Atest(3,col+1);
			//--Here this reserve ingredient is tried
			if(this->add_ingredient(A,Atest,x,b,i)){
				++col;
				group_norm.push_back(std::get<0>(elements[elements.size()-1]).get_flavor_magnitude());
				success = true;
				break;
			}
		}
		//--here if no single added ingredient can fix the problem
		if(!success && col < 2){
			for(eindex i = 0; i < reserves.size()-1; ++i) {
				for(eindex j = i+1; j < reserves.size(); ++j) {
					CMatrix Atest(3,col+2);
					if(this->add_ingredient(A,Atest,x,b,i,j)) {
						col+=2;
						group_norm.push_back(std::get<0>(elements[elements.size()-2]).get_flavor_magnitude());
						group_norm.push_back(std::get<0>(elements[elements.size()-1]).get_flavor_magnitude());
						success = true;
						break;
					}
				}
			}
		}
		if(!success) {
			this->give_up();
			return;
		}
	} catch(const multiple_solutions &e) {
		std::cerr << e.what() << std::endl;
		//--Send to optimizer
		if(!find_optimum(x,A,b)) {
			this->give_up();
			return;
		}
	}
	
	//--fill results into elements
	for(eindex i = 0; i < elements.size(); ++i) 
		std::get<1>(elements[i]) = x(std::get<2>(elements[i]))
					               *(group_norm[std::get<2>(elements[i])]
					    		     /std::get<0>(elements[i]).get_flavor_magnitude());
	return;
}

bool Cocktail::add_ingredient(const CMatrix &A, CMatrix &Anew, Eigen::VectorXd &x, 
							  const Eigen::Vector3d &b, const eindex i, const eindex j) {
	//--Construct new matrix
	for(eindex gindex = 0; gindex < eindex(A.cols()); ++gindex) {
				Anew.col(gindex) = A.col(gindex);
	}
	Anew.col(A.cols()) << reserves[i].get_alcoholic_bite(),
				          reserves[i].get_sweetness(),
						  reserves[i].get_sourness();
	if(j) { 
		Anew.col(A.cols()+1) << reserves[j].get_alcoholic_bite(),
		     		          reserves[j].get_sweetness(),
			    			  reserves[j].get_sourness();
	}
	Eigen::ColPivHouseholderQR<CMatrix> lu(A);
	Eigen::ColPivHouseholderQR<CMatrix> lutest(Anew);
	//--Don't add unless linearly independent from other ingredients
	if(eindex(lutest.rank()) < eindex(lu.rank()) + 1 + j) return false;
	
	//--Check for solution	
	if(Anew.cols()>3 && (check_nosolutions(Anew,b) 
	   || !find_optimum(x,Anew,b))) return false; 
	if(Anew.cols()==3 && !solve_squarematrix(Anew,x,false)) return false;
	if(Anew.cols()<3 && !solve_overdetermined(Anew,x,false)) return false;
	
	//--Here we can successfully add the ingredient
	eindex currgindex=std::get<2>(elements[elements.size()-1]);
	elements.push_back(std::make_tuple(reserves[i],0,++currgindex));
	if(j) {
		currgindex=std::get<2>(elements[elements.size()-1]);
		elements.push_back(std::make_tuple(reserves[j],0,++currgindex));
	}
	return true;
}

Cocktail::eindex Cocktail::classify_ingredients() {
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

void Cocktail::give_up() {
	//--Just flag all amounts to zero
	for(auto el : elements)
		std::get<1>(el) = 0;

	std::cerr << "Can't deal with this set of ingredients" << std::endl;
}

bool solve_overdetermined(const CMatrix &A, Eigen::VectorXd &x, bool throwflag) {
	const Eigen::Vector3d b(1,1,1);
	//--find least squares solution
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(3,A.cols());
	x=svd.compute(A,Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
	double dwRelErr = ( A* x - b ).norm() / b.norm();
	if(dwRelErr>Cocktail::solprecision) {
		if(throwflag) throw no_solution("Overdetermined system with no solution");
		return false;
	}
	//--check for unphysical solution
	for(Cocktail::eindex i = 0; i < Cocktail::eindex(A.cols()); ++i) {
		if(x(i) < 0) {
			if(throwflag) throw no_solution("Overdetermined system with unphysical solution");
			return false;
		}
	}
	//--success if we reach here
	return true;
}

bool solve_squarematrix(const CMatrix &A, Eigen::VectorXd &x, bool throwflag) {
	const Eigen::Vector3d b(1,1,1);
	Eigen::ColPivHouseholderQR<Eigen::Matrix3d> lu(A);
	//--check that ingredients are linearly independent
    if(lu.rank() < 3) { 

		if(check_nosolutions(A,b)) throw no_solution("3 ingredients, but no solution");
		if(throwflag) throw multiple_solutions("3 ingredients, but not linearly independent");
		return false;
	}
	x = lu.solve(b);
	if(x(0) < 0 || x(1) < 0 || x(2) < 0) {
		if(throwflag) throw no_solution("Unphysical solution");
		return false;
	}
	double dwRelErr = ( A* x - b ).norm() / b.norm();
	if(dwRelErr>Cocktail::solprecision) {
		if(throwflag) throw no_solution("Inaccurate solution");
		return false;
	}
	//--success if we reach here
	return true;
}

bool check_nosolutions(const CMatrix &A, const Eigen::Vector3d &b) {
	//--Construct augmented matrix
	CMatrix Atest(3,A.cols()+1);
	for(Cocktail::eindex gindex = 0; gindex < Cocktail::eindex(A.cols()); ++gindex) {
				Atest.col(gindex) = A.col(gindex);
	}
	Atest.col(A.cols()) = b;
	Eigen::ColPivHouseholderQR<CMatrix> lu(A);
	Eigen::ColPivHouseholderQR<CMatrix> lutest(Atest);
	if(lu.rank() == lutest.rank()) return false;
	return true;
}

bool find_optimum(Eigen::VectorXd &x, const CMatrix &A, const Eigen::Vector3d &b) {
	double fom = 1.e9; 
	bool success = false;
	x.setZero(A.cols());
	Eigen::VectorXd bestx(A.cols());
	bestx.setZero(A.cols());
	search(0,A,x,b,success,fom,bestx); 
	x = bestx;
	return success;
}

void search(Cocktail::eindex i, const CMatrix &A, Eigen::VectorXd &x, 
			const Eigen::Vector3d &b, bool &success, double &fom, Eigen::VectorXd &bestx) {
	Eigen::Vector3d minvec;
	minvec.setZero();
	//--calculate total vector from fixed amounts so far
	for(Cocktail::eindex j = 0; j < i; ++j) {
		minvec += x(j)*A.col(j);
	}
	//--calculate 3 upper bounds and take smallest
	//--when i=n, get lower bound as well (highest of 3)
	double minubound = 1/A.col(i).maxCoeff(), maxlbound = 0;
	for(Cocktail::eindex j = 0; j < 3; ++j) {
		if(x(j) > 0) {
			double tempubound = (1-minvec(j))/A(j,i);
			if(tempubound < minubound) minubound = tempubound;
			if(tempubound > maxlbound) maxlbound = tempubound;
		}
	}
	//--compute bin size
	double binsize = 1/(2*Cocktail::tspperoz*A.col(i).maxCoeff());
	//--compute nbins
	if(minubound < 0) minubound = 0;
	Cocktail::eindex nbins = 
		Cocktail::eindex(minubound/binsize)+1;
	
	if(i < Cocktail::eindex(A.cols()-1)) {
		for(Cocktail::eindex bin = 0; bin < nbins; ++bin) {
			x(i) = bin*binsize;
			search(i+1,A,x,b,success,fom,bestx);
		}
	}
	else {
	    Cocktail::eindex minbins = 
			Cocktail::eindex(maxlbound/binsize);
		double prevErr=1, tfom;
		for(Cocktail::eindex bin = minbins; bin < nbins; ++bin) {
			x(i) = bin*binsize;
			double currErr = ( A* x - b ).norm() / b.norm();
			if(currErr>prevErr) break;
			prevErr = currErr;
			if(currErr>.05) continue; 
			success = true;
			tfom = figure_of_merit(x,A);
			if(tfom < fom) {
				fom = tfom;
				bestx = x;
			}
			//std::cout<<"err= "<<currErr<<std::endl;
			//std::cout<<"x= "<<x<<std::endl;
		}
	}
}

double figure_of_merit(const Eigen::VectorXd &x, const CMatrix &A) {
	double sum = 0;
	for(Cocktail::eindex i = 0; i < Cocktail::eindex(A.cols()-1); ++i) {
		for(Cocktail::eindex j = i+1; j < Cocktail::eindex(A.cols()); ++j) {
			sum += pow((x(i)/A.col(j).norm()-x(j)/A.col(i).norm())*A.col(i).dot(A.col(j)),2);
		}
	}
	return sum;
}

//--overloaded operators
std::ostream &operator<<(std::ostream &os, const Cocktail &item) {
	double oz = 0, tsp = 0, ml = 0;
	for(auto el : item.elements) {
		//--First convert raw oz amount to oz + tsp and to ml
		oz=std::get<1>(el);
		ml=oz*Cocktail::mlperoz;
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
		os << std::left << std::setw(35) << std::get<0>(el) 
		   << " " << std::right;
		if(oz > 0) os << oz << " oz ";
		if(tsp > 0) os << tsp << " tsp"; 
		if(ml > 0) os << std::setprecision(3) << std::setw(2) << std::right << "[" << ml << " ml]";
		os << std::endl;
	}
	return os;
}


