#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <tuple>
#include <algorithm>
#include "Ingredient.h"
#include "Recipe.h"

using std::cout;
using std::cerr;
using std::cin;
using std::clog;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ostream;
using std::istringstream;
using std::string;
using std::vector;
using std::map;
using std::pair;
using std::tuple;
using std::make_tuple;
using std::get;

typedef map<string, Ingredient> BarType;

void readStartList(BarType &);
void readRecipes(vector<Recipe> &,  BarType &);
bool analyzeRecipe(Recipe &);
void analyzeRecipes(vector<Recipe> &, BarType &);
void outputIngredientProperties(string, BarType &);
void outputIngredientMeasurements(BarType &);

int main() {
	//--Create map to store database, vector for recipes
	BarType bar;
	vector<Recipe> recipes;
	//--Read databases
	readStartList(bar);
	readRecipes(recipes,bar);
	//--Analyze recipes to get flavor vectors
	analyzeRecipes(recipes,bar);
	//--Print output file
	outputIngredientProperties("Text.txt",bar);
	//--Print lists of measurements
	outputIngredientMeasurements(bar);
	char c;
	cout << "Press any key to exit." << endl;
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
	vector<double> measurements, weights;
	//--For each unknown parameter, need the corresponding ingredient, 
	//--which flavor space direction, the rest of the sum, and the amount 
	typedef tuple<Ingredient*, size_t, double, double> parameter;
	vector<parameter> parameters;
	
	double sum = 0, amount, effnummeas = 0; 
	size_t unknownpar = 0; 
	bool mflag = true;
	Ingredient* ing;
	//--Find total alcoholic bite 
	for(size_t i = 0; i < recipe.getnumberofingredients(); ++i) {
		if(recipe.getingredientat(i)->get_alcoholic_bite()>-10) {
			sum += recipe.getamountat(i)*recipe.getingredientat(i)->get_alcoholic_bite();
			effnummeas += recipe.getamountat(i)*recipe.getingredientat(i)->get_alcoholic_bite()
				        * recipe.getingredientat(i)->get_num_alcoholic_bite_measures();
		}
		else { 
			if(unknownpar) return false; //--can't have >1 unknown par of same type
			++unknownpar;
			mflag = false;
			ing = recipe.getingredientat(i);
			amount = recipe.getamountat(i);
		}
	} 
	//--If no unknowns, we have a measure of total flavor
	if (mflag) {
		measurements.push_back(sum);
		effnummeas /= sum;
		weights.push_back(effnummeas);
	}
	//--Otherwise keep track of unknown to solve for it later (0 for bite)
	else parameters.push_back(make_tuple(ing,0,sum,amount));

	//--Find total sweetness
	sum = 0; effnummeas = 0; unknownpar = 0; mflag = true;
	for(size_t i = 0; i < recipe.getnumberofingredients(); ++i) {
		if(recipe.getingredientat(i)->get_sweetness()>-10) {
			sum+=recipe.getamountat(i)*recipe.getingredientat(i)->get_sweetness();
			effnummeas += recipe.getamountat(i)*recipe.getingredientat(i)->get_sweetness()
				        * recipe.getingredientat(i)->get_num_sweetness_measures();
		}
		else {
			if(unknownpar) return false; //--can't have >1 unknown par of same type
			++unknownpar;
			mflag = false;
			ing = recipe.getingredientat(i);
			amount = recipe.getamountat(i);
		}
	}
	//--If no unknowns, we have a measure of total flavor
	if (mflag) {
		measurements.push_back(sum);
		effnummeas /= sum;
		weights.push_back(effnummeas);
	}
	//--Otherwise keep track of unknown to solve for it later (1 for sweetness)
	else parameters.push_back(make_tuple(ing,1,sum,amount));

	//--Find total sourness
	sum = 0; effnummeas = 0; unknownpar = 0; mflag = true;
	for(size_t i = 0; i < recipe.getnumberofingredients(); ++i) {
		if(recipe.getingredientat(i)->get_sourness()>-10) {
			sum+=recipe.getamountat(i)*recipe.getingredientat(i)->get_sourness();
			effnummeas += recipe.getamountat(i)*recipe.getingredientat(i)->get_sourness()
				        * recipe.getingredientat(i)->get_num_sourness_measures();
		}
		else {
			if(unknownpar) return false; //--can't have >1 unknown par of same type
			++unknownpar;
			mflag = false;
			ing = recipe.getingredientat(i);
			amount = recipe.getamountat(i);
		}
	}
	//--If no unknowns, we have a measure of total flavor
	if (mflag) { 
		measurements.push_back(sum);
		effnummeas /= sum;
		weights.push_back(effnummeas);
	}
	//--Otherwise keep track of unknown to solve for it later (2 for sourness)
	else parameters.push_back(make_tuple(ing,2,sum,amount));

	//--Solve for unknown parameters
	if(!measurements.size()) return false;
	double bestmeasure = 0, sum_weights = 0;
	for(size_t m = 0; m < measurements.size(); ++m) {
		bestmeasure += measurements[m]*weights[m];
		sum_weights += weights[m];
	}
	bestmeasure /= sum_weights; 

	for(auto p : parameters) { 
		switch(get<1>(p)){
		case 0:
			get<0>(p)->add_ab_measurement((bestmeasure-get<2>(p))/get<3>(p));
			break;
		case 1:
			get<0>(p)->add_sw_measurement((bestmeasure-get<2>(p))/get<3>(p));
			break;
		case 2:
			get<0>(p)->add_sr_measurement((bestmeasure-get<2>(p))/get<3>(p));
			break;
		}
	}
	
	return true;
}

//--function to go through all the recipes to do the analysis
void analyzeRecipes(vector<Recipe> &recipes, BarType &bar) {
	vector<Recipe*> unprocessed_data;
	for(size_t r = 0; r < recipes.size(); ++r)
		unprocessed_data.push_back(&recipes[r]);
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
		//--Update flavor vectors
		for(auto it = bar.begin(); it != bar.end(); ++it) 
			it->second.update_flavor_vector();
	}
}

void outputIngredientProperties(string fname, BarType &bar) {
	ofstream out(fname);
	if(out) {
		for(auto it = bar.begin(); it != bar.end(); ++it) 
			it->second.print_properties(out);
	}
	out.close();
}

void outputIngredientMeasurements(BarType &bar) {
	for(auto it = bar.begin(); it != bar.end(); ++it){
		if(it->second.get_num_alcoholic_bite_measures()!=1000000) {
			ofstream outab(it->second.get_name()+"_bite.txt");
			it->second.print_alcoholic_bite(outab);
			outab.close();
		}
		if(it->second.get_num_sweetness_measures()!=1000000) {
			ofstream outsw(it->second.get_name()+"_sweet.txt");
			it->second.print_sweetness(outsw);
			outsw.close();
		}
		if(it->second.get_num_sourness_measures()!=1000000) {
			ofstream outsr(it->second.get_name()+"_sour.txt");
			it->second.print_sourness(outsr);
			outsr.close();
		}
	}
}