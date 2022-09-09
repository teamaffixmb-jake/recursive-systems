#pragma once
#include "affix-base/pch.h"
#include "aurora-v4/aurora.h"

namespace recursive_systems
{
	enum class recurrence_function : uint8_t
	{
		function_identity = 0,
		function_not = 1,
		function_or = 2,
		function_and = 3,
	};

	struct recurrence_relation
	{
		recurrence_function m_recurrence_function;
		size_t m_operand_index_0 = 0;
		size_t m_operand_index_1 = 0;
		static recurrence_relation random(
			const size_t& a_system_size
		)
		{
			CryptoPP::AutoSeededRandomPool l_random;

			return recurrence_relation
			{
				recurrence_function(l_random.GenerateWord32(0, 3)),
				l_random.GenerateWord32(0, a_system_size - 1),
				l_random.GenerateWord32(0, a_system_size - 1)
			};

		}
		std::vector<double> vectorize(

		)
		{
			std::vector<double> l_result(3);
			l_result[0] = (uint8_t)m_recurrence_function;
			l_result[1] = m_operand_index_0;
			l_result[2] = m_operand_index_1;
			return l_result;
		}
		bool evaluate(
			const std::vector<bool>& a_current_state
		)
		{
			switch (m_recurrence_function)
			{
				case recurrence_function::function_identity:
				{
					return a_current_state[m_operand_index_0];
				}

				case recurrence_function::function_not:
				{
					return !a_current_state[m_operand_index_0];
				}

				case recurrence_function::function_or:
				{
					return a_current_state[m_operand_index_0] || a_current_state[m_operand_index_1];
				}

				case recurrence_function::function_and:
				{
					return a_current_state[m_operand_index_0] && a_current_state[m_operand_index_1];
				}

				default:
				{
					throw std::exception("Reached default in switch statement");
				}

			}
		}
	};

	struct recursive_system
	{
		std::vector<recurrence_relation> m_recurrence_relations;
		static recursive_system random(
			const size_t& a_system_size
		)
		{
			std::vector<recurrence_relation> l_recurrence_relations(a_system_size);
			for (int i = 0; i < l_recurrence_relations.size(); i++)
				l_recurrence_relations[i] = recurrence_relation::random(a_system_size);
			return recursive_system{ l_recurrence_relations };
		}
		std::vector<double> vectorize(

		)
		{
			std::vector<double> l_result;
			for (auto& l_recurrence_relation : m_recurrence_relations)
			{
				auto l_vectorized_recurrence_relation = l_recurrence_relation.vectorize();
				l_result.insert(l_result.end(), l_vectorized_recurrence_relation.begin(), l_vectorized_recurrence_relation.end());
			}
			return l_result;
		}
		std::vector<bool> evaluate(
			const std::vector<bool>& a_current_state
		)
		{
			std::vector<bool> l_result(m_recurrence_relations.size());
			for (int i = 0; i < l_result.size(); i++)
			{
				l_result[i] = m_recurrence_relations[i].evaluate(a_current_state);
			}
			return l_result;
		}
	};

}
