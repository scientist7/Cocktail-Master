#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include "Ingredient.h"
#include "Cocktail.h"
#include "no_solution.h"

using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::ostream;
using std::string;
using std::vector;
using std::multimap;

typedef multimap<string, Ingredient> BarType;

//--Declare functions
void readDB(BarType &);
vector<Ingredient> getIngredients(BarType &);
vector<Ingredient> getReserveIngredients(BarType &);
string SeparateWords(string text);
void printIntro();


int main() { 
	//--Create map to store database
	BarType bar;
	//--Read database
	readDB(bar);

	char command;

	do {
		//--Ask user for ingredients and create cocktail object
		Cocktail cocktail(getIngredients(bar), getReserveIngredients(bar));
		//--Calculate correct proportions
		cocktail.balance_drink();
		//--Print recipe
		cout << endl << cocktail << endl;
	    cout << "Enter q to quit, or anything else to continue" << endl;
		cin >> command;
	} while(command != 'q');

	return 0;
}

//--Define functions

//--This function reads database and stores info about known ingredients
void readDB(BarType &bar) {
	//--Open database file
	ifstream input("Text.txt");
	//--Read one ingredient at a time
	while(!input.eof()) {
		string category, name;
		double alcoholic_bite, sweetness, sourness;
		input >> category >> name >> alcoholic_bite >> sweetness >> sourness;
		//--Find uppercase and insert space between words
		category = SeparateWords(category);
		name = SeparateWords(name);
		//--Create Ingredient object and add it to map
		bar.emplace(category, 
			        Ingredient(category, name, alcoholic_bite, sweetness, sourness));
	}	
}

//--This function asks user to enter a set of ingredients and returns a vector of them
vector<Ingredient> getIngredients(BarType &bar) {

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
	
	printIntro();

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
		//--Add to cocktail vector if not duplicate
		if(!count(cocktail.cbegin(),cocktail.cend(),ing_it->second))
			cocktail.push_back(ing_it->second);	
	} 

	return cocktail;
}

//--This function returns ingredients used in case of no solution
vector<Ingredient> getReserveIngredients(BarType &bar) {
	//--vector to hold reserve ingredients
	vector<Ingredient> reserves;

	//--Define reserve ingredients
	const string reservecat[3]={"Juice","SimpleSyrup","Vodka"};
	const string reservename[3]={"Lemon","1:1Caster","BlueIce"}; 
	//--Look for these in bar
	for(int i=0; i<3; ++i) {
		for(auto bar_it = bar.lower_bound(reservecat[i]); 
				bar_it != bar.upper_bound(reservecat[i]);
				++bar_it) {
					if((bar_it->second).get_name()==reservename[i]) {
						reserves.push_back(bar_it->second);
						break;
					}
		}
	}

	return reserves;

}

string SeparateWords(string text)
{
	if (text.length() < 1) return text;
	string newText;
	newText.append(1,text[0]);
        for (int i = 1; i < text.length(); ++i)
        {
            if (isupper(text[i]) && text[i - 1] != ' ')
                newText.append(" ");
            newText.append(1,text[i]);
        }
        return newText;
}

void printIntro() { 
string graphic[15] = {
"        Welcome",
"          to",
"    Cocktail Master"
"	                  ",
"	                  ",
"     ____________",  
"    |            |",
"    \\            /",
"      -.__  __.-  ",
"          ..        ",
"          ||",
"          ||",
"          ||",
"       _.-  -._ ",
"      <________>",
};

	cout << std::right << endl ;
    for( const auto& line : graphic) cout << line << endl ;
    cout << '\n' ;

}
