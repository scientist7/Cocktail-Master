#include "Ingredient.h"

std::ostream &operator<<(std::ostream &os, const Ingredient &item) {
	os << item.get_name()+" "+item.get_category();
	return os;
}

