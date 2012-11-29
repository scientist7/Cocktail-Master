#include "Ingredient.h"
#include <string>

//--overload relational operators
bool operator==(const Ingredient &lhs, const Ingredient &rhs) {
	return lhs.get_category() == rhs.get_category() &&
		   lhs.get_name() == rhs.get_name();
}

bool operator!=(const Ingredient &lhs, const Ingredient &rhs) {
	return !(lhs == rhs);
}

std::ostream &operator<<(std::ostream &os, const Ingredient &item) {
	os << item.get_name() << " " << item.get_category();
	return os;
}