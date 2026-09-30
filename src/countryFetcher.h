#pragma once
#include "CountryData.h"
#include "ofMain.h"
#include "ofxJSON.h"

class CountryFetcher {
public:
	CountryFetcher();
	~CountryFetcher();

	void fetchCountryData(const std::string & countryName);
	CountryData getCountryData() const { return currentCountry; }
	bool hasData() const { return dataLoaded; }
	std::string getErrorMessage() const { return errorMessage; }
	bool parseJSON(const std::string & jsonResponse);

	// Async response handler for ofLoadURLAsync
	void urlResponse(ofHttpResponse & response);

private:
	CountryData currentCountry;
	bool dataLoaded;
	std::string errorMessage;
};
