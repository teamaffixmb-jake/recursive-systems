#include "aurora-v4/aurora.h"
#include "models.h"
#include "assistance_functions.h"
#include "structures.h"

using namespace aurora;



void test_train_future_state_predictor(

)
{
	using namespace recursive_systems;
	
	auto l_system = input(100);
	auto l_initial_state = input(100);
	auto l_time = input(1);

	std::vector<size_t> l_system_embedder_dimensions = { 
		l_system.size(),
		l_system.size() 
	};
	std::vector<size_t> l_time_embedder_dimensions = { 
		5,
		10 
	};
	std::vector<size_t> l_state_predictor_dimensions = { 
		l_system.size() * 2,
		l_system.size() * 2,
		(size_t)(l_system.size() * 1.5) 
	};

	element_vector::start();
	parameter_vector::start();
	
	auto l_future_state = predict_future_state(
		pointers(l_system),
		pointers(l_initial_state),
		pointers(l_time),
		l_system_embedder_dimensions,
		l_time_embedder_dimensions,
		l_state_predictor_dimensions
	);

	element_vector l_element_vector = element_vector::stop();
	parameter_vector l_parameter_vector = parameter_vector::stop(-1, 1);



}

void test_recursive_system(

)
{
	using namespace recursive_systems;

	size_t l_system_size = 4;

	recursive_system l_recursive_system = recursive_system::random(l_system_size);

	std::vector<bool> l_state(l_system_size);
	randomize(l_state);

	std::vector<bool> l_next_state = l_recursive_system.evaluate(l_state);

}

int main(

)
{
	test_recursive_system();
	
}
