#include "Ingredient.h"

void Ingredient::update_flavor_vector() {
	if(alcoholic_bite_measurements.size())
		alcoholic_bite = std::accumulate(alcoholic_bite_measurements.cbegin(),
										 alcoholic_bite_measurements.cend(),
										 0.)/alcoholic_bite_measurements.size();	

	if(sweetness_measurements.size()) 
		sweetness = std::accumulate(sweetness_measurements.cbegin(),
									sweetness_measurements.cend(),
									0.)/sweetness_measurements.size();

	if(sourness_measurements.size())
		sourness = std::accumulate(sourness_measurements.cbegin(),
									sourness_measurements.cend(),
									0.)/sourness_measurements.size();
}

void Ingredient::print_properties(std::ostream & os) {
	os << category << " " << name << " " << alcoholic_bite 
		      << " " << sweetness << " " << sourness << std::endl;
}

std::ostream &operator<<(std::ostream &os, const Ingredient &item) {
	os << item.get_name()+" "+item.get_category();
	return os;
}

