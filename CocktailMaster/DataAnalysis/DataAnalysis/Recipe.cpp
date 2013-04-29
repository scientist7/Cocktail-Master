#include "Recipe.h"

double Recipe::mlperoz = 30;

Recipe::Recipe(const std::vector<Ingredient*> ins, 
		   const std::vector<double> ams) {
	for(size_t i = 0; i < ins.size(); ++i) {
		components.push_back(std::make_tuple(ins[i],ams[i]/mlperoz));
	}
}

//--Calculate sum of alcoholic bite in recipe, identify unknown parameters
size_t Recipe::check_alcoholic_bite_sum(double &sum, double &effnummeas, size_t &index) {
	size_t unknownpar = 0;
	double amount = 0;
	//--Find total alcoholic bite
	for(size_t i = 0; i < this->getnumberofingredients(); ++i) {
		if(this->getingredientat(i)->get_alcoholic_bite()>-10) {
			sum += this->getamountat(i)*this->getingredientat(i)->get_alcoholic_bite();
			effnummeas += this->getamountat(i)*this->getingredientat(i)->get_alcoholic_bite()
				        * this->getingredientat(i)->get_num_alcoholic_bite_measures();
		}
		else { 
			++unknownpar;
			index = i; //--get assigned the index of last unknown parameter
		}
	} 
	effnummeas /= sum;
	
	return unknownpar;
}

//--Calculate sum of sweetness in recipe, identify unknown parameters
size_t Recipe::check_sweetness_sum(double &sum, double &effnummeas, size_t &index) {
	size_t unknownpar = 0;
	double amount = 0;
	//--Find total sweetness
	for(size_t i = 0; i < this->getnumberofingredients(); ++i) {
		if(this->getingredientat(i)->get_sweetness()>-10) {
			sum += this->getamountat(i)*this->getingredientat(i)->get_sweetness();
			effnummeas += this->getamountat(i)*this->getingredientat(i)->get_sweetness()
				        * this->getingredientat(i)->get_num_sweetness_measures();
		}
		else { 
			++unknownpar;
			index = i; //--get assigned the index of last unknown parameter
		}
	} 
	effnummeas /= sum;
	
	return unknownpar;
}

//--Calculate sum of sourness in recipe, identify unknown parameters
size_t Recipe::check_sourness_sum(double &sum, double &effnummeas, size_t &index) {
	size_t unknownpar = 0;
	double amount = 0;
	//--Find total sweetness
	for(size_t i = 0; i < this->getnumberofingredients(); ++i) {
		if(this->getingredientat(i)->get_sourness()>-10) {
			sum += this->getamountat(i)*this->getingredientat(i)->get_sourness();
			effnummeas += this->getamountat(i)*this->getingredientat(i)->get_sourness()
				        * this->getingredientat(i)->get_num_sourness_measures();
		}
		else { 
			++unknownpar;
			index = i; //--get assigned the index of last unknown parameter
		}
	} 
	effnummeas /= sum;
	
	return unknownpar;
}

std::ostream &operator<<(std::ostream &os, const Recipe &recipe) {
	
	for(auto component : recipe.components) {
		os << std::left << std::setw(35) << *std::get<0>(component) 
		   << " " << std::right << std::get<1>(component) << std::endl;
	}
	return os;
}
