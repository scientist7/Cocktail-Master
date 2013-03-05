#pragma once
#include <string>
class Ingredient
{
public:
	Ingredient(std::string ct, std::string nm, double ab, double sw, double sr) :                                                    
		       category(ct), name(nm), alcoholic_bite(ab), sweetness(sw), sourness(sr) {}
	
private:
	std::string category;
	std::string name;
	double alcoholic_bite;
	double sweetness;
	double sourness;
	std::vector<double> alcoholic_bite_measurements;
	std::vector<double> sweetness_measurements;
	std::vector<double> sourness_measurements;
};

