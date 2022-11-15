#include "aurora-v4/aurora.h"
#include "recursive_systems.h"
#include <filesystem>
#include <iostream>
#include "affix-base/files.h"
#include "affix-base/serializable.h"
#include <Windows.h>

using namespace aurora;

inline std::string to_string(
	const std::vector<bool>& a_binary_vector
)
{
	std::string l_result;
	for (int i = 0; i < a_binary_vector.size(); i++)
		l_result += std::to_string(a_binary_vector[i]);
	return l_result;
}

inline std::vector<bool> bitwise_xor(
	const std::vector<bool>& a_x0,
	const std::vector<bool>& a_x1
)
{
	assert(a_x0.size() == a_x1.size());
	std::vector<bool> l_result(a_x0.size());
	for (int i = 0; i < l_result.size(); i++)
		l_result[i] = a_x0[i] != a_x1[i];
	return l_result;
}

inline std::vector<bool> flatten(
	const std::vector<std::vector<bool>>& a_x
)
{
	std::vector<bool> l_result(a_x.size() * a_x[0].size());
	for (int i = 0; i < a_x.size(); i++)
		for (int j = 0; j < a_x[0].size(); j++)
			l_result[i * a_x[0].size() + j] = a_x[i][j];
	return l_result;
}

inline std::vector<bool> random_binary_vector(
	const size_t& a_size
)
{
	std::vector<bool> l_result(a_size);
	for (int i = 0; i < l_result.size(); i++)
		l_result[i] = rand() % 2;
	return l_result;
}

inline std::vector<std::vector<bool>> random_minterm_matrix(
	const size_t& a_system_size
)
{
	std::vector<std::vector<bool>> l_result(a_system_size);
	const size_t l_matrix_width = (size_t)pow(2, a_system_size);
	for (int i = 0; i < l_result.size(); i++)
	{
		l_result[i] = random_binary_vector(l_matrix_width);
	}
	return l_result;
}

inline std::vector<double> convert(
	const std::vector<bool>& a_binary_vector
)
{
	std::vector<double> l_result(a_binary_vector.size());
	for (int i = 0; i < a_binary_vector.size(); i++)
		l_result[i] = a_binary_vector[i];
	return l_result;
}

inline std::vector<std::vector<double>> convert(
	const std::vector<std::vector<bool>>& a_minterm_matrix
)
{
	std::vector<std::vector<double>> l_result(a_minterm_matrix.size());
	for (int i = 0; i < l_result.size(); i++)
		l_result[i] = convert(a_minterm_matrix[i]);
	return l_result;
}

size_t path_count_in_directory(
	const std::filesystem::path& a_path
)
{
	if (!std::filesystem::is_directory(a_path))
		throw std::exception("Error: path is not a valid directory.");

	size_t l_count = 0;

	for (auto l_path : std::filesystem::directory_iterator(a_path))
		l_count++;

	return l_count;

}

std::vector<std::filesystem::path> paths_in_directory(
	const std::filesystem::path& a_path
)
{
	if (!std::filesystem::is_directory(a_path))
		throw std::exception("Error: path is not a valid directory.");

	std::vector<std::filesystem::path> l_result;

	for (auto l_path : std::filesystem::directory_iterator(a_path))
		l_result.push_back(l_path);

	return l_result;

}

class training_set
{
public:
	std::vector<double> m_x;
	std::vector<double> m_y;
	training_set(

	)
	{

	}
	training_set(
		const std::vector<double>& a_x,
		const std::vector<double>& a_y
	) :
		m_x(a_x),
		m_y(a_y)
	{

	}
	bool serialize(
		affix_base::data::byte_buffer& a_byte_buffer
	) const
	{
		if (!a_byte_buffer.push_back(m_x))
			return false;
		if (!a_byte_buffer.push_back(m_y))
			return false;
		return true;
	}
	bool deserialize(
		affix_base::data::byte_buffer& a_byte_buffer
	)
	{
		if (!a_byte_buffer.pop_front(m_x))
			return false;
		if (!a_byte_buffer.pop_front(m_y))
			return false;
		return true;
	}
};

void generate_training_data(

)
{

	srand(5);
	const size_t l_system_size = 5;
	const size_t ITEMS_PER_DIRECTORY = 10000;
	const size_t FUTURE_DURATION_TIL_PREDICT = 1000;

	const std::string l_folder_path = "training_data/" + std::to_string(l_system_size) + "_system/";
	
	if (std::filesystem::exists(l_folder_path) && !std::filesystem::is_directory(l_folder_path))
		throw std::exception("Folder name was a file.");

	// If the folder does not yet exist, create it.
	if (!std::filesystem::exists(l_folder_path))
		std::filesystem::create_directory(l_folder_path);

	for (int i = path_count_in_directory(l_folder_path); i < 10000; i++)
	{
		std::string l_file_path = l_folder_path + std::to_string(i) + "_" + std::to_string(rand()) + ".bin";
		{
			auto l_initial_state = random_binary_vector(l_system_size);

			auto l_unmodulated_matrix = random_minterm_matrix(l_system_size);
			auto l_unmodulated_system = recursive_systems::recursive_system(
				l_unmodulated_matrix,
				l_initial_state
			);


			auto l_modulated_matrix = random_minterm_matrix(l_system_size);

			auto l_modulated_system = recursive_systems::recursive_system(
				l_modulated_matrix,
				l_initial_state
			);


			auto l_unmodulated_state = l_unmodulated_system.next(FUTURE_DURATION_TIL_PREDICT);
			auto l_modulated_state = l_modulated_system.next(FUTURE_DURATION_TIL_PREDICT);


			auto l_flattened_matrix_xor = bitwise_xor(flatten(l_unmodulated_matrix), flatten(l_modulated_matrix));
			auto l_state_xor = bitwise_xor(l_unmodulated_state, l_modulated_state);

			training_set l_training_set(
				aurora::oneshot::concat(
					aurora::oneshot::concat(
						convert(l_initial_state),
						aurora::oneshot::flatten(convert(l_unmodulated_matrix))
					),
					convert(l_flattened_matrix_xor)
				),
				convert(l_state_xor)
			);


			if (affix_base::files::file_write(l_file_path, l_training_set))
				std::cout << "SUCCESSFULLY WROTE TRAINING SET TO FILE: " << l_file_path << std::endl;

		}

		{
			training_set l_recovered_training_set;
			Sleep(1000);
			
			// I think file_read is fucked up and should be rewritten (affix_base)

			if (affix_base::files::file_read(l_file_path, l_recovered_training_set))
				std::cout << "SUCCESSFULLY RECOVERED TRAINING SET FROM FILE: " << l_file_path << std::endl;

		}

	}




}




const std::vector<size_t> NEURAL_NET_DIMENSIONS = { 40, 20, 10 };

inline std::vector<double> predict_future_state_change(
	aurora::oneshot::parameter_vector& a_parameter_vector,
	const std::vector<double>& a_x
)
{
	using namespace aurora::oneshot;

	// Reset the next parameter index to be retrieved.
	a_parameter_vector.next_index(0);

	// Create the result vector
	std::vector<double> l_result(a_x);

	for (int i = 0; i < NEURAL_NET_DIMENSIONS.size() - 1; i++)
	{
		l_result = multiply(
			a_parameter_vector.next(NEURAL_NET_DIMENSIONS[i], l_result.size()), l_result);
		l_result = add(
			a_parameter_vector.next(NEURAL_NET_DIMENSIONS[i]), l_result);
		l_result = leaky_relu(
			l_result, 0.3);
	}

	l_result = multiply(
		a_parameter_vector.next(NEURAL_NET_DIMENSIONS.back(), l_result.size()), l_result);
	l_result = add(
		a_parameter_vector.next(NEURAL_NET_DIMENSIONS.back()), l_result);
	l_result = sigmoid(
		l_result);

	return l_result;

}

inline double model_fitness(
	aurora::oneshot::parameter_vector& a_parameter_vector,
	const training_set& a_training_set
)
{
	std::vector<double> l_y = predict_future_state_change(a_parameter_vector, a_training_set.m_x);
	return 1.0 / (oneshot::mean_squared_error(l_y, a_training_set.m_y) + 0.000001);
}

inline bool random_training_set(
	const std::vector<std::filesystem::path>& a_training_set_file_paths,
	training_set& a_result
)
{
	size_t l_random_training_set_index = rand() % a_training_set_file_paths.size();
	if (!affix_base::files::file_read(a_training_set_file_paths[l_random_training_set_index].generic_u8string(), a_result))
		return false;
	return true;
}

inline bool random_training_sets(
	const std::vector<std::filesystem::path>& a_training_set_file_paths,
	std::vector<training_set>& a_result
)
{
	for (int i = 0; i < a_result.size(); i++)
	{
		if (!random_training_set(a_training_set_file_paths, a_result[i]))
			return false;
	}
	return true;
}

void train_AI_predict_future_state_change(
	const std::filesystem::path& a_training_data_folder_path,
	const std::filesystem::path& a_training_session_file_path
)
{
	using namespace aurora::oneshot;

	// Ensure that the training data folder exists
	if (!std::filesystem::exists(a_training_data_folder_path))
		throw std::exception("Error: training data folder does not exist.");

	// Store a list of the training set paths in the training data folder (this will act as an index)
	// so we can rapidly reuse it later (during training).
	std::vector<std::filesystem::path> l_training_set_file_paths(paths_in_directory(a_training_data_folder_path));
	
	const size_t PARTICLE_COUNT = 50;

	std::vector<parameter_vector> l_positions(PARTICLE_COUNT);

	{
		// Attempt to import a dummy training set as input for param vector building stage
		training_set l_dummy_training_set;
		if (!random_training_set(l_training_set_file_paths, l_dummy_training_set))
			throw std::exception("Error: failed to import dummy training set for param vector building.");

		// Build param vectors randomly
		for (int i = 0; i < PARTICLE_COUNT; i++)
		{
			parameter_vector_builder l_builder(-1, 1);
			predict_future_state_change(l_builder, l_dummy_training_set.m_x);
			l_positions[i] = l_builder;
		}
	}

	// WE HAVE NOW BUILT THE PARAMETER VECTORS



	std::vector<particle_optimizer> l_particle_optimizers;

	for (parameter_vector& l_position : l_positions)
		l_particle_optimizers.push_back(particle_optimizer(l_position));

	particle_swarm_optimizer l_optimizer(l_particle_optimizers, 0.9, 0.2, 0.8);



	// Restore the training session.
	if (std::filesystem::exists(a_training_session_file_path))
		if (!affix_base::files::file_read(a_training_session_file_path.generic_u8string(), l_optimizer))
			throw std::exception("Error: failed to read training session from file.");



	std::vector<double> l_particle_rewards(PARTICLE_COUNT);

	const size_t MINI_BATCH_SIZE = 32;

	const size_t TRAINING_SET_CACHE_SIZE = 1000;

	const size_t SAVE_TRAINING_SESSION_INTERVAL = 100;

	std::vector<training_set> l_training_set_cache(TRAINING_SET_CACHE_SIZE);

	if (!random_training_sets(l_training_set_file_paths, l_training_set_cache))
		throw std::exception("Error: failed to import training set cache.");

	for (int l_epoch = 0; true; l_epoch++)
	{
		for (int l_train_index = 0; l_train_index < MINI_BATCH_SIZE; l_train_index++)
		{
			// Get a random index in the training set cache.
			size_t l_training_set_index = rand() % l_training_set_cache.size();

			// Loop through all particles, and get their fitness on the
			// randomly selected training set.
			for (int l_particle_index = 0; l_particle_index < PARTICLE_COUNT; l_particle_index++)
			{
				l_particle_rewards[l_particle_index] += 
					model_fitness(l_positions[l_particle_index], l_training_set_cache[l_training_set_index]);
			}
		}
		
		l_optimizer.update(l_particle_rewards);

		for (int i = 0; i < l_particle_rewards.size(); i++)
			// Clear the particle's reward to avoid over-accumulation.
			l_particle_rewards[i] = 0;

		if (l_epoch % SAVE_TRAINING_SESSION_INTERVAL == 0)
			// Save the training session at a set interval.
			affix_base::files::file_write(a_training_session_file_path.generic_u8string(), l_optimizer);
		
	}


}

int main(

)
{
	generate_training_data();
	/*train_AI_predict_future_state_change(
		"training_data/5_system/",
		"training_sessions/5_system_attempt_0.bin"
	);*/
}
