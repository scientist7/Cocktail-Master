#include "Ingredient.h"
#include <string>

double CosAngleIngredients(const Ingredient &lhs, const Ingredient &rhs) {
	double mag1 = sqrt(lhs.get_alcoholic_bite()*lhs.get_alcoholic_bite()
				     + lhs.get_sourness()*lhs.get_sourness()
				     + lhs.get_sweetness()*lhs.get_sweetness());
	double mag2 = sqrt(rhs.get_alcoholic_bite()*rhs.get_alcoholic_bite()
				     + rhs.get_sourness()*rhs.get_sourness()
				     + rhs.get_sweetness()*rhs.get_sweetness());
	double dotprod = lhs.get_alcoholic_bite()*rhs.get_alcoholic_bite()
				   + lhs.get_sourness()*rhs.get_sourness()
				   + lhs.get_sweetness()*rhs.get_sweetness();

	return dotprod/(mag1*mag2);
}

bool collinear(const Ingredient &lhs, const Ingredient &rhs) {
	//--if they are close enough, consider them collinear
	if(fabs(CosAngleIngredients(lhs,rhs)-1) < .00001) return true;
	else return false;

}

//--overload relational operators
bool operator==(const Ingredient &lhs, const Ingredient &rhs) {
	return lhs.get_category() == rhs.get_category() &&
		   lhs.get_name() == rhs.get_name();
}

bool operator!=(const Ingredient &lhs, const Ingredient &rhs) {
	return !(lhs == rhs);
}

std::ostream &operator<<(std::ostream &os, const Ingredient &item) {
	os << item.get_name()+" "+item.get_category();
	return os;
}