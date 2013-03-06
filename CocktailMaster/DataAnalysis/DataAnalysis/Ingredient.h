#pragma once
#include <string>
#include <vector>
class Ingredient
{
public:
	Ingredient(std::string ct, std::string nm, double ab, double sw, double sr) :                                                    
		       category(ct), name(nm), alcoholic_bite(ab), sweetness(sw), sourness(sr) {}
	
	void add_ab_measurement(double ab) {alcoholic_bite_measurements.push_back(ab);}
	void add_sw_measurement(double sw) {sweetness_measurements.push_back(sw);}
	void add_sr_measurement(double sr) {sourness_measurements.push_back(sr);}

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

