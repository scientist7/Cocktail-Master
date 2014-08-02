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

void readStartList(BarType &, string);
void readRecipes(vector<Recipe> &,  BarType &, string);
bool analyzeRecipe(Recipe &);
void analyzeRecipes(vector<Recipe> &, BarType &);
void outputIngredientProperties(string, BarType &);
void outputIngredientMeasurements(BarType &, string);
void outputRecipeMeasurements(vector<Recipe> &, string);

int main() {
	//--Create map to store database, vector for recipes
	BarType bar;
	vector<Recipe> recipes;
	//--Read databases
	string Dir="./";  //--set directory to read and write
	readStartList(bar,Dir);
	readRecipes(recipes,bar,Dir);
	//--Analyze recipes to get flavor vectors
	analyzeRecipes(recipes,bar);
	//--Print output file
	outputIngredientProperties(Dir+"Text.txt",bar);
	//--Print lists of measurements
	outputIngredientMeasurements(bar,Dir);
    outputRecipeMeasurements(recipes,Dir); 

	char c;
	cout << "Press any key to exit." << endl;
	cin >> c;
	return 0;
}

//--function to read list of ingredients with assumptions about 
//--their properties
void readStartList(BarType &bar, string Dir) {
	//--Open database file
	ifstream input(Dir+"StartingBarList.txt"); 
	//--Read one ingredient at a time
	while(!input.eof()) {
		string category, name;
		double alcoholic_bite, sweetness, sourness;
		input >> category >> name >> alcoholic_bite >> sweetness >> sourness;
		//--Create Ingredient object and add it to map
		bar.emplace(category+"-"+name, 
			        Ingredient(category, name, alcoholic_bite, sweetness, sourness));
	}		
	input.close();
}

//--function to read list of well tested cocktail recipes
void readRecipes(vector<Recipe> &recipes, BarType &bar, string Dir) {
	string line,temp; 
	size_t numIng;
	bool badRecipe;
	//--Open list of recipes
	ifstream input(Dir+"RecipeList.txt");
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
				cerr << "Ingredient: " << temp << " not found in database!" << endl;
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
	
	double sum = 0, effnummeas = 0; 
	size_t unknownpar = 0, index = 300; 

	//--Find total alcoholic bite 
	unknownpar = recipe.check_alcoholic_bite_sum(sum,effnummeas,index);
	 //--Don't deal with >1 free parameter
	if(unknownpar > 1) return false; 
	//--Here record info for one free parameter
	else if(unknownpar == 1) {
		parameters.push_back(make_tuple(recipe.getingredientat(index),0,
			                            sum,recipe.getamountat(index)));
	}
	//--If no unknowns, record a measure of total flavor
	else {
		measurements.push_back(sum);
		weights.push_back(effnummeas);
	}
	
	//--Find total sweetness
	sum = 0; effnummeas = 0; index = 300;
	unknownpar = recipe.check_sweetness_sum(sum,effnummeas,index);
	 //--Don't deal with >1 free parameter
	if(unknownpar > 1) return false; 
	//--Here record info for one free parameter
	else if(unknownpar == 1) {
		parameters.push_back(make_tuple(recipe.getingredientat(index),1,
			                            sum,recipe.getamountat(index)));
	}
	//--If no unknowns, record a measure of total flavor
	else {
		measurements.push_back(sum);
		weights.push_back(effnummeas);
	}

	//--Find total sourness
	sum = 0; effnummeas = 0; index = 300;
	unknownpar = recipe.check_sourness_sum(sum,effnummeas,index);
	 //--Don't deal with >1 free parameter
	if(unknownpar > 1) return false; 
	//--Here record info for one free parameter
	else if(unknownpar == 1) {
		parameters.push_back(make_tuple(recipe.getingredientat(index),2,
			                            sum,recipe.getamountat(index)));
	}
	//--If no unknowns, record a measure of total flavor
	else {
		measurements.push_back(sum);
		weights.push_back(effnummeas);
	}


	//--Too many free parameters 
	if(!measurements.size()) return false;

	//--Get best estimate of average flavor
	double best_measure = 0, sum_weights = 0;
	for(size_t m = 0; m < measurements.size(); ++m) {
		best_measure += measurements[m]*weights[m];
		sum_weights += weights[m];
	}
	best_measure /= sum_weights; 

	//--If no free parameters, use recipe as additional info
	//--on all ingredients
	if(!parameters.size()) {
		//--First identify unconstrained flavor parameters
		vector<double> unconstrained_sum(3,0);
		for(size_t i = 0; i < recipe.getnumberofingredients(); ++i) {
			if(recipe.getingredientat(i)->get_num_alcoholic_bite_measures() 
			   != Ingredient::num_meas_fixed)
			   unconstrained_sum[0] += recipe.getingredientat(i)->get_alcoholic_bite()
									 * recipe.getamountat(i);

			if(recipe.getingredientat(i)->get_num_sweetness_measures() 
			   != Ingredient::num_meas_fixed)
			   unconstrained_sum[1] += recipe.getingredientat(i)->get_sweetness()
									 * recipe.getamountat(i);

			if(recipe.getingredientat(i)->get_num_sourness_measures() 
			   != Ingredient::num_meas_fixed)
			   unconstrained_sum[2] += recipe.getingredientat(i)->get_sourness()
									 * recipe.getamountat(i);
		}

	    //--Then adjust all unconstrained flavor parameters according to this recipe;
		//--this constitutes another measurement

		//--Need scale factors for 3 flavor types
		vector<double> scale_adjust(3);
		for(size_t i = 0; i < 3; ++i) {
			scale_adjust[i] = (best_measure - (measurements[i] - unconstrained_sum[i]))
				              /unconstrained_sum[i];
		}
		//--Then calculate a new measurement for all unconstrained flavor parameters
		for(size_t i = 0; i < recipe.getnumberofingredients(); ++i) {
			if(recipe.getingredientat(i)->get_num_alcoholic_bite_measures() 
			   != Ingredient::num_meas_fixed)
			   recipe.getingredientat(i)->add_ab_measurement(scale_adjust[0]
			                                                 *recipe.getingredientat(i)->get_alcoholic_bite());

			if(recipe.getingredientat(i)->get_num_sweetness_measures() 
			   != Ingredient::num_meas_fixed)
			   recipe.getingredientat(i)->add_sw_measurement(scale_adjust[1]
			                                                 *recipe.getingredientat(i)->get_sweetness());

			if(recipe.getingredientat(i)->get_num_sourness_measures() 
			   != Ingredient::num_meas_fixed)
			   recipe.getingredientat(i)->add_sr_measurement(scale_adjust[2]
			                                                 *recipe.getingredientat(i)->get_sourness());
		}
	}

	//--Otherwise solve for unknown parameters
	else {
		double measure;
		for(auto p : parameters) {
			measure = (best_measure-get<2>(p))/get<3>(p);
			switch(get<1>(p)){
			case 0:
				get<0>(p)->add_ab_measurement(measure>0 ? measure : 0);
				break;
			case 1:
				get<0>(p)->add_sw_measurement(measure>0 ? measure : 0);
				break;
			case 2:
				get<0>(p)->add_sr_measurement(measure>0 ? measure : 0);
				break;
			}
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
	while(unprocessed_data.size() && passcounter<10) {
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
			if(it->second.get_alcoholic_bite() > -10
				&& it->second.get_sweetness() > -10
				&& it->second.get_sourness() > -10) {

				it->second.print_properties(out);
			}
	}
	out.close();
}

void outputIngredientMeasurements(BarType &bar, string Dir) {
	for(auto it = bar.begin(); it != bar.end(); ++it) {
		if(it->second.get_num_alcoholic_bite_measures()!=Ingredient::num_meas_fixed &&
			it->second.get_num_alcoholic_bite_measures()) {
			ofstream outab(Dir+it->second.get_name()+it->second.get_category()+"_bite.txt");
			it->second.print_alcoholic_bite(outab);
			outab.close();
		}
		if(it->second.get_num_sweetness_measures()!=Ingredient::num_meas_fixed &&
			it->second.get_num_sweetness_measures()) {
			ofstream outsw(Dir+it->second.get_name()+it->second.get_category()+"_sweet.txt");
			it->second.print_sweetness(outsw);
			outsw.close();
		}
		if(it->second.get_num_sourness_measures()!=Ingredient::num_meas_fixed &&
			it->second.get_num_sourness_measures()) {
			ofstream outsr(Dir+it->second.get_name()+it->second.get_category()+"_sour.txt");
			it->second.print_sourness(outsr);
			outsr.close();
		}
	}
}

void outputRecipeMeasurements(vector<Recipe> &recipes, string Dir) {
	double alcoholic_bite = 0, sweetness = 0, sourness = 0;
	bool incomplete_recipe = false;
	ofstream abswdiff(Dir+"alcoholic_bite_minus_sweetness.txt");
	ofstream absrdiff(Dir+"alcoholic_bite_minus_sourness.txt");
	ofstream srswdiff(Dir+"sourness_minus_sweetness.txt");
	//--loop over recipes
	for(auto it = recipes.begin(); it != recipes.end(); ++it) {
		alcoholic_bite = 0, sweetness = 0, sourness = 0;
		incomplete_recipe = false;
		//--Calculate total bite,sweetness,sourness for each recipe
		for(size_t i = 0; i < it->getnumberofingredients(); ++i) {
			if(it->getingredientat(i)->get_alcoholic_bite() > -10 
		    	&& it->getingredientat(i)->get_sweetness() > -10 
				&& it->getingredientat(i)->get_sourness() > -10){
					alcoholic_bite += it->getamountat(i)*it->getingredientat(i)->get_alcoholic_bite();
					sweetness += it->getamountat(i)*it->getingredientat(i)->get_sweetness();
					sourness += it->getamountat(i)*it->getingredientat(i)->get_sourness();
			}
			else {
				incomplete_recipe = true;
				break;
			}
		}
		if(!incomplete_recipe){
			abswdiff << 2 * (alcoholic_bite - sweetness) / (alcoholic_bite + sweetness)  << endl;
			absrdiff << 2 * (alcoholic_bite - sourness) / (alcoholic_bite + sourness) << endl;
			srswdiff << 2 * (sourness - sweetness) / (sourness + sweetness) << endl;
		}
	}
	abswdiff.close();
	absrdiff.close();
	srswdiff.close();
}