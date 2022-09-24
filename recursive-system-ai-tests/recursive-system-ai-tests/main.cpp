#include "aurora-v4/aurora.h"
#include "assistance_functions.h"
#include "structures.h"
#include <iostream>

using namespace aurora;

void test_train_future_state_predictor(

)
{
	using namespace recursive_systems;

	size_t l_system_size = 20;
	size_t l_future_size = 100000;

	recursive_system l_recursive_system = recursive_system::random(l_system_size);

	std::vector<std::vector<bool>> l_future(l_future_size);

	randomize(l_future[0]);

	for (int i = 1; i < l_future_size; i++)
	{
		l_future[i] = l_recursive_system.evaluate(l_future[i - 1]);
	}

	// Create persistent threads for parallel execution
	std::vector<affix_base::threading::persistent_thread> l_persistent_threads(std::thread::hardware_concurrency() / 2);

	std::vector<size_t> l_state_predictor_hidden_dimensions = { 5, l_system_size * 5 };



}

void test_recursive_system(

)
{
	using namespace recursive_systems;

	size_t l_system_size = 10;

	recursive_system l_recursive_system = recursive_system::random(l_system_size);

	std::vector<bool> l_state(l_system_size);
	randomize(l_state);

	std::vector<bool> l_next_state = l_recursive_system.evaluate(l_state);
	


}

int main(

)
{
	test_train_future_state_predictor();
	
}
