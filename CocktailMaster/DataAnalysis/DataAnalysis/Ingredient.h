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
	
	void add_ab_measurement(double ab, size_t rindex) {
		//--Add measurement if this recipe hasn't already been used for this component
		if(find(ab_recipe_indices.cbegin(), ab_recipe_indices.cend(), rindex) 
			==  ab_recipe_indices.cend()) {
			alcoholic_bite_measurements.push_back(ab);
			ab_recipe_indices.push_back(rindex);
		}
	}
	void add_sw_measurement(double sw, size_t rindex) {
		//--Add measurement if this recipe hasn't already been used for this component
		if(find(sw_recipe_indices.cbegin(), sw_recipe_indices.cend(), rindex) 
			==  sw_recipe_indices.cend()) {
			sweetness_measurements.push_back(sw);
			sw_recipe_indices.push_back(rindex);
		}
	}
	void add_sr_measurement(double sr, size_t rindex) {
		//--Add measurement if this recipe hasn't already been used for this component
		if(find(sr_recipe_indices.cbegin(), sr_recipe_indices.cend(), rindex) 
			==  sr_recipe_indices.cend()) {
			sourness_measurements.push_back(sr);
			sr_recipe_indices.push_back(rindex);
		}
	}
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
	//--These are the number of measurements used in current calculated flavor
	double get_num_alcoholic_bite_measures() const { return num_alcoholic_bite_measures; }
	double get_num_sweetness_measures() const { return num_sweetness_measures; }
	double get_num_sourness_measures() const { return num_sourness_measures; }
	//--Function to check if all parameters have >0 measurements
	bool are_measures_avail() const { return (alcoholic_bite_measurements.size() || alcoholic_bite > -10) &&
									  (sweetness_measurements.size() || sweetness > -10) &&
									  (sourness_measurements.size() || sourness > -10); }
	
	
	static const size_t num_meas_fixed = 10000000;

private:
	const std::string category;
	const std::string name;
	double alcoholic_bite;
	double sweetness;
	double sourness;
	std::vector<double> alcoholic_bite_measurements;
	std::vector<size_t> ab_recipe_indices;
	std::vector<double> sweetness_measurements;
	std::vector<size_t> sw_recipe_indices;
	std::vector<double> sourness_measurements;
	std::vector<size_t> sr_recipe_indices;
	size_t num_alcoholic_bite_measures;
	size_t num_sweetness_measures;
	size_t num_sourness_measures;
};

//--overloaded operators
std::ostream &operator<<(std::ostream &os, const Ingredient &item);