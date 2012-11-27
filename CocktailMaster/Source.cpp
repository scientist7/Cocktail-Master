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
using std::multimap;

//--Declare functions
void readDB(multimap<string, Ingredient> &);
vector<Ingredient> GetIngredients(multimap<string, Ingredient> &);

int main() {
	//--Create map to store database
	multimap<string, Ingredient> bar;
	//--Read database
	readDB(bar);
	//--Ask user for ingredients
	vector<Ingredient> cocktail = GetIngredients(bar);
	return 0;
}

//--Define functions

//--This function reads database and stores info about known ingredients
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

//--This function asks user to enter a set of ingredients and returns a vector of them
vector<Ingredient> GetIngredients(multimap<string, Ingredient> &bar) {

	//--vector to hold ingredients
	vector<Ingredient> cocktail;

	//--Loop to enter as many ingredients as user wants
	int catchoice, prodchoice; 
	std::size_t catlabel;

	//--Make a vector of unique keys for faster access later
	vector<string> Categories;
	for(auto bar_it = bar.cbegin(); bar_it != bar.cend(); 
			bar_it = bar.upper_bound(bar_it->first)) {
			Categories.push_back(bar_it->first);
	}
	
	while(1) {
		cout<<"Please select an ingredient category, or 0 if done"<<endl;	
		catlabel=1;
		for(auto element : Categories) {
			cout<<"("<<catlabel<<") "<<element<<endl;
			++catlabel;
		}
		cin >> catchoice;

		//--Exit on catchoice<1
		if(catchoice<1) break;

		//--Otherwise list products in selected category
		catlabel=1;
		cout<<"Please choose a product"<<endl;
		for(auto bar_it = bar.lower_bound(Categories[catchoice-1]); 
			bar_it != bar.upper_bound(Categories[catchoice-1]);
			++bar_it) {
			cout<<"("<<catlabel<<") "<<(bar_it->second).get_name()<<endl;
			++catlabel;
		}
		cin >> prodchoice;
	
		//--Exit on prodchoice<1
		if(prodchoice<1) break;
	
		//--Otherwise add ingredient to cocktail
		auto ing_it=bar.find(Categories[catchoice-1]);
		advance(ing_it,prodchoice-1);
	    cocktail.push_back(ing_it->second);	
	} 

	return cocktail;
}

