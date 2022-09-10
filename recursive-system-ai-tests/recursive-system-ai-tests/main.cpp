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

	std::vector<bool> l_initial_state(l_system_size);

	randomize(l_initial_state);

	recursive_system l_recursive_system = recursive_system::random(l_system_size);

	std::vector<std::vector<bool>> l_future(l_future_size);

	std::vector<bool> l_state = l_initial_state;

	l_future[0] = l_initial_state;

	for (int i = 1; i < l_future_size; i++)
	{
		l_state = l_recursive_system.evaluate(l_state);
		l_future[i] = l_state;
	}

	// Create persistent threads for parallel execution
	std::vector<affix_base::threading::persistent_thread> l_persistent_threads(std::thread::hardware_concurrency() / 2);
	parallel_executor::startup(l_persistent_threads);

	// Begin creation of the model
	element_vector::start();
	parameter_vector::start();

	std::vector<size_t> l_state_predictor_hidden_dimensions = { 5, l_system_size * 5 };

	size_t l_next_parameter_index = parameter_vector::next_index();

	auto l_x = input(l_persistent_threads.size(), 1);

	std::vector<std::vector<state_gradient_pair*>> l_y = pointers(l_x);

	join_threads();

	for (int i = 0; i < l_persistent_threads.size(); i++)
	{
		parameter_vector::next_index(l_next_parameter_index);

		element_vector::start();

		// Begin building actual model.

		auto& l_thread_specific_y = l_y[i];

		for (const size_t& l_dimension : l_state_predictor_hidden_dimensions)
		{
			l_thread_specific_y = weight_junction(l_thread_specific_y, l_dimension);
			l_thread_specific_y = bias(l_thread_specific_y);
			l_thread_specific_y = leaky_relu(l_thread_specific_y, 0.3);
		}

 		l_thread_specific_y = weight_junction(l_thread_specific_y, l_system_size);
		l_thread_specific_y = bias(l_thread_specific_y);
		l_thread_specific_y = leaky_relu(l_thread_specific_y, 0.3);

		parallel_branch(l_persistent_threads[i], element_vector::stop());

	}

	join_threads();

	auto l_desired_y = input(l_persistent_threads.size(), l_system_size);
	auto l_loss = mean_squared_error(l_y, pointers(l_desired_y))->depend();

	// Stop creation of the model
	element_vector l_element_vector = element_vector::stop();
	parameter_vector l_parameter_vector = parameter_vector::stop(-0.1, 0.1);

	gradient_descent l_optimizer(l_parameter_vector, 0.2);

	CryptoPP::AutoSeededRandomPool l_random;

	for (int epoch = 0; true; epoch++)
	{
		for (int i = 0; i < l_persistent_threads.size(); i++)
		{
			size_t l_future_index = l_random.GenerateWord32(0, l_future.size() - 1);
			l_x[i][0].m_state = l_future_index;
			set_state(l_desired_y[i], l_future[l_future_index]);
		}

		l_element_vector.fwd();
		l_loss.m_partial_gradient = 1;
		l_element_vector.bwd();

		l_optimizer.update();

		if (epoch % 1000 == 0 && epoch != 0)
		{
			std::cout << l_loss.m_state << std::endl;
		}

	}

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
