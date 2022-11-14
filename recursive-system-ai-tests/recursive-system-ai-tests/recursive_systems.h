#pragma once
#include "affix-base/pch.h"
#include "aurora-v4/aurora.h"

namespace recursive_systems
{
	/// <summary>
	/// An input that gets taken from a previously inputted system state, which may or may not be inverted.
	/// </summary>
	class binary_literal
	{
	public:
		uint64_t m_index = 0;
		bool m_invert = false;

	public:
		binary_literal(
			const size_t& a_index,
			const bool& a_invert
		) :
			m_index(a_index),
			m_invert(a_invert)
		{

		}
		bool evaluate(
			const std::vector<bool>& a_state
		)
		{
			// Perform XOR between the inversion signal and the input substate.
			return m_invert != a_state[m_index];
		}
	};

	/// <summary>
	/// A list of (possibly) inverted binary inputs, which when put through a product, will result in a minterm.
	/// </summary>
	class minterm : public std::vector<binary_literal>
	{
	public:
		minterm(
			const size_t& a_system_size,
			const size_t& a_minterm_index
		)
		{
			for (int i = 0; i < a_system_size; i++)
			{
				// Perform a bit shift before AND-ing the result with mintermIndex, so as to reveal
				// whether or not an inversion should occur on the given binary input.
				bool l_invert = (a_minterm_index & ((size_t)pow(2, a_system_size - 1) >> i)) == 0;
				// Construct binaryinput
				push_back(binary_literal(i, l_invert));
			}
		}
		bool evaluate(
			const std::vector<bool>& a_state
		)
		{
			for (int i = 0; i < size(); i++)
			{
				if (!at(i).evaluate(a_state))
					// If EVER the evaluation of a binary input is false, we can return false, seeing as 
					// a minterm is made up of repeated ANDs of the (inverted)/(not inverted) binary inputs.
					return false;
			}
			return true;
		}
	};

	/// <summary>
	/// A list of minterms which are included in a minterm sum, so as to produce a binary operator.
	/// </summary>
	class binary_operator : public std::vector<minterm>
	{
	public:
		binary_operator(
			const std::vector<bool>& a_minterm_sum
		)
		{
			size_t l_system_size = log2(a_minterm_sum.size());
			for (int i = 0; i < a_minterm_sum.size(); i++)
			{
				if (a_minterm_sum[i])
					push_back(minterm(l_system_size, i));
			}
		}
		bool evaluate(
			const std::vector<bool>& a_state
		)
		{
			for (int i = 0; i < size(); i++)
			{
				if (at(i).evaluate(a_state))
					// Return true if ONE minterm evaluates to true because we are OR-ing all of the minterms
					return true;
			}
			return false;
		}
	};

	/// <summary>
	/// A class which holds multiple binary operators. Each operator acts on inputs from the previous timestep 
	/// to evaluate the current timestep.
	/// </summary>
	class recursive_system : public std::vector<binary_operator>
	{
	private:
		std::vector<bool> m_state;

	public:
		recursive_system(
			const std::vector<std::vector<bool>>& a_minterm_matrix,
			const std::vector<bool>& a_initial_state
		) :
			m_state(a_initial_state)
		{
			for (int i = 0; i < a_minterm_matrix.size(); i++)
				push_back(binary_operator(a_minterm_matrix[i]));
		}
		std::vector<bool> next(
			const size_t& a_iterations = 1
		)
		{
			std::vector<bool> l_result(m_state.size());

			// Simulates recursive system for the number of timesteps indicated.
			for (int l_iteration = 0; l_iteration < a_iterations; l_iteration++)
			{
				// Loop through all binary operators, evaluating them on the previous state.
				for (int i = 0; i < l_result.size(); i++)
					l_result[i] = at(i).evaluate(m_state);

				// Save the current state for the next time calling next().
				m_state = l_result;
			}

			return m_state;

		}
	};
}
