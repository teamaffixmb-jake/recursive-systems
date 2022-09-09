#pragma once
#include "affix-base/pch.h"
#include "aurora-v4/aurora.h"
#include "cryptopp/osrng.h"

namespace recursive_systems
{
	inline void set_state(
		std::vector<aurora::state_gradient_pair>& a_destination,
		const std::vector<bool>& a_source
	)
	{
		assert(a_destination.size() == a_source.size());
		for (int i = 0; i < a_destination.size(); i++)
			a_destination[i].m_state = a_source[i];
	}

	inline void randomize(
		std::vector<bool>& a_state
	)
	{
		CryptoPP::AutoSeededRandomPool l_random;

		for (int i = 0; i < a_state.size(); i++)
			a_state[i] = l_random.GenerateWord32(0, 1);
	}

}
