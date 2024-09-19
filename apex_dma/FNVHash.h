#pragma once

#include <cstdlib>
#include <cstdint>
#include <functional>
#include <string>

class FnvHash {
public:
	size_t operator()(const std::string& s) const {
		const size_t fnv_prime = 0x100000001b3;
		size_t hash = 0xcbf29ce484222325; // FNV offset basis
		for (char c : s) {
			hash ^= c;
			hash *= fnv_prime;
		}
		return hash;
	}
};