#pragma once
#include "aurora-v4/aurora.h"
#include "affix-base/serializable.h"

namespace recursive_systems
{
	inline std::vector<aurora::state_gradient_pair*> predict_future_state(
		std::vector<aurora::state_gradient_pair*> a_system,
		std::vector<aurora::state_gradient_pair*> a_initial_state,
		std::vector<aurora::state_gradient_pair*> a_time,
		std::vector<size_t> a_system_embedder_dimensions,
		std::vector<size_t> a_time_embedder_dimensions,
		std::vector<size_t> a_state_predictor_dimensions
	)
	{
		using namespace aurora;

		std::vector<state_gradient_pair*> l_system_embedding = a_system;
		std::vector<state_gradient_pair*> l_time_embedding = a_time;

		for (auto& l_system_embedder_dimension : a_system_embedder_dimensions)
		{
			l_system_embedding = weight_junction(l_system_embedding, l_system_embedder_dimension);
			l_system_embedding = bias(l_system_embedding);
			l_system_embedding = leaky_relu(l_system_embedding, 0.3);
		}

		for (auto& l_time_embedder_dimension : a_time_embedder_dimensions)
		{
			l_time_embedding = weight_junction(l_time_embedding, l_time_embedder_dimension);
			l_time_embedding = bias(l_time_embedding);
			l_time_embedding = leaky_relu(l_time_embedding, 0.3);
		}

		std::vector<state_gradient_pair*> l_state_prediction = concat(l_system_embedding, a_initial_state);
		l_state_prediction = concat(l_state_prediction, l_time_embedding);

		for (int i = 0; i < a_state_predictor_dimensions.size(); i++)
		{
			l_state_prediction = weight_junction(l_state_prediction, a_state_predictor_dimensions[i]);
			l_state_prediction = bias(l_state_prediction);
			l_state_prediction = leaky_relu(l_state_prediction, 0.3);
		}

		l_state_prediction = weight_junction(l_state_prediction, a_initial_state.size());
		l_state_prediction = bias(l_state_prediction);
		l_state_prediction = sigmoid(l_state_prediction);

		return l_state_prediction;

	}

}
