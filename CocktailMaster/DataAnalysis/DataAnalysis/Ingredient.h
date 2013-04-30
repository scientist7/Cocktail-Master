#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
class Ingredient
{
public:
	Ingredient(std::string ct, std::string nm, double ab, double sw, double sr) :                                                    
		       category(ct), name(nm), alcoholic_bite(ab), sweetness(sw), sourness(sr),
			   num_alcoholic_bite_measures((ab>-10)?num_meas_fixed:0), 
			   num_sweetness_measures((sw>-10)?num_meas_fixed:0), 
			   num_sourness_measures((sr>-10)?num_meas_fixed:0) {}
	
	void add_ab_measurement(double ab) {alcoholic_bite_measurements.push_back(ab);}
	void add_sw_measurement(double sw) {sweetness_measurements.push_back(sw);}
	void add_sr_measurement(double sr) {sourness_measurements.push_back(sr);}
	void update_flavor_vector();
	void print_properties(std::ostream &os);
	void print_alcoholic_bite(std::ostream &os);
	void print_sweetness(std::ostream &os);
	void print_sourness(std::ostream &os);
	
	std::string get_category() const { return category; }
	std::string get_name() const { return name; }
	double get_alcoholic_bite() const { return alcoholic_bite; }
	double get_sweetness() const { return sweetness; }
	double get_sourness() const { return sourness; }
	double get_num_alcoholic_bite_measures() const { return num_alcoholic_bite_measures; }
	double get_num_sweetness_measures() const { return num_sweetness_measures; }
	double get_num_sourness_measures() const { return num_sourness_measures; }
	
	static size_t num_meas_fixed;

private:
	const std::string category;
	const std::string name;
	double alcoholic_bite;
	double sweetness;
	double sourness;
	std::vector<double> alcoholic_bite_measurements;
	std::vector<double> sweetness_measurements;
	std::vector<double> sourness_measurements;
	size_t num_alcoholic_bite_measures;
	size_t num_sweetness_measures;
	size_t num_sourness_measures;
};

//--overloaded operators
std::ostream &operator<<(std::ostream &os, const Ingredient &item);