#include "Cocktail.h"

//--define static members
const double Cocktail::ozincrements=0.25;
const double Cocktail::mlincrements=5;
const double Cocktail::tspperoz=6;
const double Cocktail::solprecision=0.001;
const double Cocktail::scaleprecision=0.01;
const double Cocktail::mlperoz=30;

//--Non-member stuff
double round_to_multiple(const double input, 
						 const double multiple) {
	//--Round to nearest increment
	double remainder = fmod(input,multiple);
	return input - remainder 
		   + floor((remainder/multiple) + 0.5)*multiple;		 
}


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
		if(!success){ 
			for(eindex i = 0; i < reserves.size()-1; ++i) {
				for(eindex j = i+1; j < reserves.size(); ++j) {
					CMatrix Atest(3,col+2); 
					if(this->add_ingredient(A,Atest,x,b,i,j)) { 
						col+=2;
						group_norm.push_back(std::get<0>(elements[elements.size()-2]).get_flavor_magnitude());
						group_norm.push_back(std::get<0>(elements[elements.size()-1]).get_flavor_magnitude());
						success = true;
						break; //--break out of inner loop
					}
				}
				if(success) break; //--break out of outer loop
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

void Cocktail::scale_recipe() {
	//--Goal is to scale recipe to get even measures on the ingredients
	//--while preserving the proportions accurately

	//--Place upper bound, that sum of alcoholic ingredients <= scale_bounds[1] oz
	//--lower bound, at scale_bounds[0] oz
	//--Ideal scaling give total of 2 oz
	double booze_total = 0, scale_bounds[2], ideal_scale;
	for(eindex i = 0; i < elements.size(); ++i) {
		if(std::get<0>(elements[i]).get_alcoholic_bite())
			booze_total += std::get<1>(elements[i]);
	}
	if(!booze_total) {
		std::cerr << "NO BOOZE IN RECIPE!" << std::endl;
		return;
	}
	scale_bounds[0] = 1.0/booze_total;
	scale_bounds[1] = 2.5/booze_total;
	ideal_scale = 2.0/booze_total;

	//--Fill ingredient matrix and vector
	CMatrix A(3,elements.size());
	Eigen::VectorXd x(elements.size()), result(elements.size());
	const Eigen::Vector3d b(1,1,1);
	//--loop to fill A & x
	for(eindex i = 0; i < elements.size(); ++i) {
		A.col(i) << std::get<0>(elements[i]).get_alcoholic_bite(),
				    std::get<0>(elements[i]).get_sweetness(),
					std::get<0>(elements[i]).get_sourness();
	}
	

	//--Search range of scale factors on recipe and choose the best
	bool rounded_to_zero = false;
	double total_discrepancy, min_scale_dev = 100, curr_scale_dev, best_scale = 1, best_discrepancy = 100;
	for(double scale = scale_bounds[0]; scale <= scale_bounds[1]; scale = scale + 0.025) {
		rounded_to_zero = false;
        //--Loop through ingredients
		for(eindex i = 0; i < elements.size(); ++i) {
			x(i) = ::round_to_multiple(scale*std::get<1>(elements[i])*Cocktail::mlperoz, 
				                       Cocktail::mlincrements)/Cocktail::mlperoz;
			//--Flag whenever something gets rounded to zero
			if(!x(i)) {
				rounded_to_zero = true;
				break;
			}
		} 
		//--Keep trying if we've rounded something to zero
		if(rounded_to_zero) continue;
		//--Calulate discrepancy from ideal solution
		result = A*x;
		result /= result.mean();
		total_discrepancy = ( result - b ).norm() / b.norm();
		//--Here we have an acceptable discrepancy
		if(total_discrepancy < Cocktail::scaleprecision) {
			curr_scale_dev = fabs(scale - ideal_scale);
			if(curr_scale_dev < min_scale_dev) {
				min_scale_dev = curr_scale_dev;
			    best_scale = scale;
			}
			else break;
		}
		//--If no discrepancy meets threshold, we choose the minimum
		else {
			if(total_discrepancy < best_discrepancy){
				best_discrepancy = total_discrepancy;
				best_scale = scale;
			}
		}
	}

	if(min_scale_dev == 100) 
		std::cerr << "Using scaling with least error = " << best_discrepancy << std::endl; 

	//--Here we have a good rescale factor, apply it to recipe
	for(eindex i = 0; i < elements.size(); ++i) {
		std::get<1>(elements[i]) *= best_scale;
	}

	return;
}

bool Cocktail::add_ingredient(const CMatrix &A, CMatrix &Anew, Eigen::VectorXd &x, 
							  const Eigen::Vector3d &b, const eindex i, const eindex j) {
	//--Don't add anything collinear with existing elements
	for(eindex test = 0; test < elements.size(); ++test) {
		if(collinear(reserves[i],std::get<0>(elements[test]))) return false;
		if(j && collinear(reserves[j],std::get<0>(elements[test]))) return false;
	}
	
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
	
	//--Check that solutions exist
	if(check_nosolutions(A,b)) {
		if(throwflag) throw no_solution("3 ingredients, but no solution");
		else return false;
	}

	//--Here, there is at least one solution, check whether it's unique
	if (fabs(A.determinant())<Cocktail::solprecision)
		if(throwflag) throw multiple_solutions("3 ingredients, multiple solutions");

	//--Here solution must exist and it unique
	x = lu.solve(b);
	//--Check that solution is physical
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
	Cocktail::candsolutions solutions;
	search(0,A,x,b,success,bestx,solutions); 
	double tfom,locmin1err=0;
	//--Search candidate solutions from best to worst until 1st local min in fom is found
	std::multimap<double, Eigen::VectorXd>::iterator it;
	for(it=solutions.begin(); it != solutions.end(); ++it) {
			std::cout<<"Error = "<<it->first<<std::endl;
			tfom = figure_of_merit(it->second,A);
			std::cout<<"-----fom = "<<tfom<<std::endl;
			if(tfom>fom) break;
			fom=tfom;
			bestx=it->second;
			locmin1err=it->first;
	}
	//--Search for any nearby solutions with lower fom
	const double maxErrorInc = .02;
	++it;
	while(it != solutions.end()) {
		if(it->first-locmin1err>maxErrorInc) break;
		std::cout<<"Error = "<<it->first<<std::endl;
		tfom = figure_of_merit(it->second,A);
		std::cout<<"-----fom = "<<tfom<<std::endl;
		//--Require a decrease in fom by some fraction of fom
		const double fomdecthresh=.05;
		if(tfom<(1.-fomdecthresh)*fom) {
			fom=tfom;
			bestx=it->second;
		}
		++it;
	}
	std::cout<<"best fom= "<<fom<<std::endl;
	x = bestx;
	return success;
}

void search(Cocktail::eindex i, const CMatrix &A, Eigen::VectorXd &x, 
			const Eigen::Vector3d &b, bool &success, Eigen::VectorXd &bestx, 
			Cocktail::candsolutions &solutions) {
	const double maxError=.1;
	Eigen::Vector3d minvec;
	minvec.setZero();
	//--calculate total vector from fixed amounts so far
	for(Cocktail::eindex j = 0; j < i; ++j) {
		minvec += x(j)*A.col(j);
	}
	//--calculate 3 upper bounds and take smallest
	//--when i=n-1, get lower bound as well (highest of 3)
	double minubound = 1/A.col(i).maxCoeff(), maxlbound=0;
	for(Cocktail::eindex j = 0; j < 3; ++j) {
		if(x(j) > 0 && A(j,i) > 0) {
			double tempubound = (1-minvec(j))/A(j,i);
			if(tempubound < minubound) minubound = tempubound;
			if(i == Cocktail::eindex(A.cols()-1) 
			   && tempubound > maxlbound) maxlbound = tempubound;
		}
	}
	//--compute bin size
	double binsize = 1/(2*Cocktail::tspperoz*A.col(i).maxCoeff());
	
	//--compute nbins
	if(minubound < 0) minubound = 0;
	//--Allow search to extend one bin beyond that expected, which may still have small error
	Cocktail::eindex nbins = 
		Cocktail::eindex(minubound/binsize)+2;
	//--force each ingredient to be >0
	//--Allow search to extend one bin beyond that expected, which may still have small error
	Cocktail::eindex minbins = 
			std::max(int(maxlbound/binsize)-1,1);

	if(i < Cocktail::eindex(A.cols()-1)) {
		for(Cocktail::eindex bin = minbins; bin < nbins; ++bin) {
			x(i) = bin*binsize;
			search(i+1,A,x,b,success,bestx,solutions);
		}
	}
	else {
		double prevErr=1;
		for(Cocktail::eindex bin = minbins; bin < nbins; ++bin) {
			x(i) = bin*binsize;
			double currErr = ( A* x - b ).norm() / b.norm();
			if(currErr>prevErr) break;
			prevErr = currErr;
			if(currErr>maxError) continue;
			solutions.insert(std::pair<double,Eigen::VectorXd>(currErr,x));
			success = true;
		}
	}
	
	bestx=x;
}

double figure_of_merit(const Eigen::VectorXd &x, const CMatrix &A) {
	double sum = 0;
	for(Cocktail::eindex i = 0; i < Cocktail::eindex(A.cols()-1); ++i) {
		for(Cocktail::eindex j = i+1; j < Cocktail::eindex(A.cols()); ++j) {
			//--New way just minimizes difference in total flavors regardless of angle
			sum += (pow((x(i)*A.col(i).norm()-x(j)*A.col(j).norm()),2));
			//--Old way weighs difference in flavor by cos(theta)
			//sum += pow((x(i)/A.col(j).norm()-x(j)/A.col(i).norm())*A.col(i).dot(A.col(j)),2);
		}
	}
	return sum;
}

//--overloaded operators
std::ostream &operator<<(std::ostream &os, const Cocktail &item) {
	double oz = 0, tsp = 0, ml = 0, mlrem = 0, exact = 0;
	os << "INGREDIENT" << std::string(60,' ') << "APPROX [oz,tsp]"
	   << std::string(10,' ') << "APPROX [ml]" << std::string(10,' ') 
	   << "EXACT [ml]" << std::endl;
	os << std::string(130,'-') << std::endl;
	for(auto el : item.elements) {
		//--First convert raw oz amount to oz + tsp and to ml
		oz = std::get<1>(el);
		ml = oz*Cocktail::mlperoz;
		exact = ml;
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
		//--Round to nearest tsp
		tsp *= Cocktail::tspperoz;
		tsp = round_to_multiple(tsp,1);
		
		//--Round to nearest ml increment
		ml = round_to_multiple(ml,Cocktail::mlincrements);


		os << std::left << std::setw(65) << std::get<0>(el) 
		   << " " << std::right;
		if(oz > 0 || tsp > 0) os << std::setw(10) << oz << " oz " << tsp << " tsp"; 
		if(ml > 0) os << std::setprecision(3) << std::setw(18) << std::right << ml << " ml";
		if(exact > 0) os <<std::setprecision(3) << std::setw(17) << exact << " ml";
		os << std::endl;
	}
	return os;
}


