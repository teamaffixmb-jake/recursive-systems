#include "aurora-v4/aurora.h"
#include "recursive_systems.h"
#include <filesystem>
#include <iostream>
#include "affix-base/files.h"
#include "affix-base/serializable.h"

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

struct training_set : public affix_base::data::serializable
{
public:
	std::vector<double> m_x;
	std::vector<double> m_y;
	training_set(

	) :
		affix_base::data::serializable(
			m_x,
			m_y
		)
	{

	}
	training_set(
		const std::vector<double>& a_x,
		const std::vector<double>& a_y
	) :
		affix_base::data::serializable(
			m_x,
			m_y
		),
		m_x(a_x),
		m_y(a_y)
	{

	}
};

void generate_training_data(

)
{

	srand(5);
	const size_t l_system_size = 3;
	const size_t ITEMS_PER_DIRECTORY = 10000;
	const size_t FUTURE_DURATION_TIL_PREDICT = 1000;


	const std::string l_folder_path = std::to_string(l_system_size) + "_system/";
	
	if (std::filesystem::exists(l_folder_path) && !std::filesystem::is_directory(l_folder_path))
		throw std::exception("Folder name was a file.");

	// If the folder does not yet exist, create it.
	if (!std::filesystem::exists(l_folder_path))
		std::filesystem::create_directory(l_folder_path);

	for (int i = path_count_in_directory(l_folder_path); i < 10000; i++)
	{
		auto l_initial_state = random_binary_vector(l_system_size);

		auto l_unmodulated_matrix = random_minterm_matrix(l_system_size);
		auto l_unmodulated_system = recursive_systems::recursive_system(
			l_unmodulated_matrix,
			l_initial_state
		);


		auto l_modulated_matrix = l_unmodulated_matrix;
		// Modulate the original matrix in some random way
		const size_t l_row_to_modulate = rand() % l_modulated_matrix.size();
		const size_t l_col_to_modulate = rand() % l_modulated_matrix[0].size();

		// Invert the bit
		l_modulated_matrix[l_row_to_modulate][l_col_to_modulate] =
			!l_modulated_matrix[l_row_to_modulate][l_col_to_modulate];

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

		std::string l_file_path = l_folder_path + std::to_string(i) + "_" + std::to_string(rand()) + ".bin";

		if (affix_base::files::file_write(l_file_path, l_training_set))
			std::cout << "SUCCESSFULLY WROTE TRAINING SET TO FILE: " << l_file_path << std::endl;

	}




}

void AI_predict_future_state_change(

)
{


}

int main(

)
{
	generate_training_data();
}
