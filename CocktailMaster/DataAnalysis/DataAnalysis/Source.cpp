#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include "Ingredient.h"

using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::ostream;
using std::string;
using std::vector;
using std::map;

typedef map<string, Ingredient> BarType;

void readStartList(BarType &);

int main() {
	//--Create map to store database
	BarType bar;
	//--Read database
	readStartList(bar);

	return 0;
}

//--function to read list of ingredients with assumptions about 
//--their properties
void readStartList(BarType &bar) {
	//--Open database file
	ifstream input("StartingBarList.txt");
	//--Read one ingredient at a time
	while(!input.eof()) {
		string category, name;
		double alcoholic_bite, sweetness, sourness;
		input >> category >> name >> alcoholic_bite >> sweetness >> sourness;
		//--Create Ingredient object and add it to map
		bar.emplace(name, 
			        Ingredient(category, name, alcoholic_bite, sweetness, sourness));
	}	
}