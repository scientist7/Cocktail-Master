#pragma once
#include <iostream>

class Ingredient
{
public:
	//--Constructors
	Ingredient() :
		       category(""), name(""), alcoholic_bite(0), sweetness(0), sourness(0) {}
	Ingredient(std::string ct, std::string nm, double ab, double sw, double sr) :                                                    
		       category(ct), name(nm), alcoholic_bite(ab), sweetness(sw), sourness(sr)  {}

	//--Functions to get ingredient properties
	std::string get_category() const { return category; }
	std::string get_name() const { return name; }
	double get_alcoholic_bite() const { return alcoholic_bite; }
	double get_sweetness() const { return sweetness; }
	double get_sourness() const { return sourness; }

private:
	std::string category;
	std::string name;
	double alcoholic_bite;
	double sweetness;
	double sourness;
};

