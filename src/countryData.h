#pragma once
#include "ofMain.h"
#include <string>
#include <vector>
#include <cstdint>

// Structure to hold country data
struct CountryData {
	std::string name;
	std::string capital;
	int64_t population;
	std::string region;
	std::string flag;
	std::vector<std::string> currencies;
	std::vector<std::string> languages;

	CountryData()
		: population(0) { }
};
