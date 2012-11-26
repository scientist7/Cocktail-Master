#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "Ingredient.h"

using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::ostream;
using std::string;
using std::multimap;

//--Declare functions
void readDB(multimap<string, Ingredient> &);

int main() {
	//--Create map to store database
	multimap<string, Ingredient> bar;
	//--Read database
	readDB(bar);
	return 0;
}

//--Define functions
void readDB(multimap<string, Ingredient> &bar) {
	//--Open database file
	ifstream input("Text.txt");
	//--Read one ingredient at a time
	while(!input.eof()) {
		string category, name;
		double alcoholic_bite, sweetness, sourness;
		input >> category >> name >> alcoholic_bite >> sweetness >> sourness;
		//--Create Ingredient object and add it to map
		bar.emplace(category, 
			        Ingredient(category, name, alcoholic_bite, sweetness, sourness));
	}	
}

