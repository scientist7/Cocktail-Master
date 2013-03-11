#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <tuple>
#include "Ingredient.h"
#include "Recipe.h"

using std::cout;
using std::cerr;
using std::cin;
using std::clog;
using std::endl;
using std::ifstream;
using std::ostream;
using std::istringstream;
using std::string;
using std::vector;
using std::map;
using std::make_tuple;

typedef map<string, Ingredient> BarType;

void readStartList(BarType &);
void readRecipes(vector<Recipe> &,  BarType &);
bool analyzeRecipe(Recipe &);
void analyzeRecipes(vector<Recipe> &);

int main() {
	//--Create map to store database, vector for recipes
	BarType bar;
	vector<Recipe> recipes;
	//--Read databases
	readStartList(bar);
	readRecipes(recipes,bar);
	analyzeRecipes(recipes);

	//--TEST
	char c;
	for(size_t i=0; i<recipes.size(); ++i) 
		cout << recipes[i] << endl;
	
	cin >> c;
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
void readRecipes(vector<Recipe> &recipes, BarType &bar) {
	string line,temp; 
	size_t numIng;
	bool badRecipe;
	//--Open list of recipes
	ifstream input("RecipeList.txt");
	while(getline(input,line)) {
		badRecipe = false;
		istringstream cocktail(line);
		cocktail >> temp;
		numIng = atoi(temp.c_str());
		vector<Ingredient*> ingredients;
		//--Read ingredient names
		for(size_t i = 0; i < numIng; ++i) {
			cocktail >> temp;
			//--Retrieve correct ingredient from database
			auto it=bar.find(temp);
			if(it == bar.end()) {
				cerr << "Ingredient: " << temp << "not found in database!" << endl;
				badRecipe = true;
				break;
			}
			//--Add to vector
			ingredients.push_back(&it->second);
		}
		if(badRecipe) continue;	
		vector<double> amounts;
		for(size_t i = 0; i < numIng; ++i) {
			cocktail >> temp;
			amounts.push_back(atof(temp.c_str()));
		}
		//--Add all this info to recipes
		recipes.emplace_back(ingredients,amounts);

	}
}

//--function to calculate unknown parameters in a recipe
bool analyzeRecipe(Recipe &recipe) {
	
	return false;
}

//--function to go through all the recipes to do the analysis
void analyzeRecipes(vector<Recipe> &recipes) {
	vector<Recipe*> unprocessed_data;
	for(auto r: recipes)
		unprocessed_data.push_back(&r);
	size_t passcounter = 0;
	

	//--Process list of recipes
	while(unprocessed_data.size() && passcounter<4) {
		vector<Recipe*> leftover_recipes;
		for(size_t i = 0; i < unprocessed_data.size(); ++i) {
			if(!analyzeRecipe(*unprocessed_data[i])) 
				leftover_recipes.push_back(unprocessed_data[i]);
		}
		++passcounter;
		clog << "Completed pass " << passcounter << ", " 
			 << leftover_recipes.size() << " recipes remain." << endl;
		unprocessed_data = leftover_recipes;
	}
}