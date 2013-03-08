#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include "Ingredient.h"
#include "Recipe.h"

using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::ostream;
using std::istringstream;
using std::string;
using std::vector;
using std::map;

typedef map<string, Ingredient> BarType;

void readStartList(BarType &);
void readRecipes(vector<Recipe> &, const BarType &);

int main() {
	//--Create map to store database, vector for recipes
	BarType bar;
	vector<Recipe> recipes;
	//--Read databases
	readStartList(bar);
	readRecipes(recipes,bar);
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

//--function to read list of well tested cocktail recipes
void readRecipes(vector<Recipe> &recipes, const BarType &bar) {
	string line,temp; 
	size_t numIng;
	BarType::iterator it;
	//--Open list of recipes
	ifstream input("RecipeList.txt");
	while(getline(input,line)) {
		istringstream cocktail(line);
		cocktail >> temp;
		numIng = atoi(temp.c_str());
		vector<Ingredient> ingredients;
		//--Read ingredient names
		for(size_t i = 0; i < numIng; ++i) {
			cocktail >> temp;
			//--Retrieve correct ingredient from database
			it = bar.find(temp);
		}
	}
}