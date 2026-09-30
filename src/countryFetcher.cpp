#include "countryFetcher.h"
#include "ofxJSON.h"

// Add your API key from restcountries.com otherwhise the API will not work.
static const std::string API_KEY = "";

// Helper: URL-encode a string for use in query paths
static std::string urlEncode(const std::string & value) {
	std::string encoded;
	char hexBuf[4];
	for (unsigned char c : value) {
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			encoded.push_back(c);
		} else if (c == ' ') {
			encoded += "%20";
		} else {
			sprintf_s(hexBuf, "%%%02X", c);
			encoded.append(hexBuf);
		}
	}
	return encoded;
}

// Helper: Convert string to lowercase for case-insensitive comparison
static std::string toLower(const std::string & str) {
	std::string lower = str;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	return lower;
}

// Store the last search query so we can match it during parsing
static std::string lastSearchQuery;

CountryFetcher::CountryFetcher()
	: dataLoaded(false) {
	ofAddListener(ofURLResponseEvent(), this, &CountryFetcher::urlResponse);
}

CountryFetcher::~CountryFetcher() {
	ofRemoveListener(ofURLResponseEvent(), this, &CountryFetcher::urlResponse);
}

void CountryFetcher::fetchCountryData(const std::string & countryName) {
	dataLoaded = false;
	errorMessage.clear();
	lastSearchQuery = countryName; // Store the search query

	std::string encoded = urlEncode(countryName);
	std::string url = "https://api.restcountries.com/countries/v5?q="
		+ encoded + "&api-key=" + API_KEY;

	ofLogNotice() << "CountryFetcher: fetching URL: " << url;
	ofLoadURLAsync(url, "countryFetch");
}

void CountryFetcher::urlResponse(ofHttpResponse & response) {
	ofLogNotice() << "CountryFetcher: HTTP status: " << response.status;

	if (response.status == 200) {
		if (parseJSON(response.data)) {
			dataLoaded = true;
			ofLogNotice() << "CountryFetcher: Data parsed successfully";
		} else {
			dataLoaded = false;
			ofLogError() << "CountryFetcher: " << errorMessage;
		}
	} else {
		dataLoaded = false;
		errorMessage = "Network error or invalid request (HTTP " + ofToString(response.status) + ")";
		ofLogError() << "CountryFetcher: " << errorMessage;
	}
}

bool CountryFetcher::parseJSON(const std::string & jsonResponse) {
	ofxJSONElement json;

	try {
		if (!json.parse(jsonResponse)) {
			errorMessage = "Failed to parse JSON string";
			return false;
		}

		if (!json.isObject() || !json.isMember("data")) {
			errorMessage = "Missing data object in API response.";
			return false;
		}

		if (!json["data"].isObject() || !json["data"].isMember("objects")) {
			errorMessage = "Missing objects array in API response.";
			return false;
		}

		ofxJSONElement objects = json["data"]["objects"];

		if (!objects.isArray() || objects.size() == 0) {
			errorMessage = "No matching countries found.";
			return false;
		}

		// Try to find an exact match on common name (case-insensitive)
		// If no exact match, use the first result
		ofxJSONElement country;
		bool foundExactMatch = false;
		std::string searchQuery = toLower(lastSearchQuery);

		for (int i = 0; i < objects.size(); i++) {
			std::string commonName = toLower(objects[i]["names"]["common"].asString());
			if (commonName == searchQuery) {
				country = objects[i];
				foundExactMatch = true;
				ofLogNotice() << "CountryFetcher: Found exact match at index " << i;
				break;
			}
		}

		// If no exact match found, use first result
		if (!foundExactMatch) {
			country = objects[0];
			ofLogNotice() << "CountryFetcher: No exact match found, using first result";
		}

		currentCountry.name = country["names"]["common"].asString();

		currentCountry.capital = "N/A";
		if (country.isMember("capitals") && country["capitals"].isArray() && country["capitals"].size() > 0) {
			currentCountry.capital = country["capitals"][0]["name"].asString();
		}

		currentCountry.population = country["population"].asInt64();
		currentCountry.region = country["region"].asString();
		currentCountry.flag = country["flag"]["url_png"].asString();

		currentCountry.currencies.clear();
		if (country.isMember("currencies") && country["currencies"].isArray()) {
			for (int i = 0; i < country["currencies"].size(); i++) {
				currentCountry.currencies.push_back(country["currencies"][i]["code"].asString());
			}
		}

		currentCountry.languages.clear();
		if (country.isMember("languages") && country["languages"].isArray()) {
			for (int i = 0; i < country["languages"].size(); i++) {
				currentCountry.languages.push_back(country["languages"][i]["name"].asString());
			}
		}

		return true;
	} catch (const std::exception & e) {
		errorMessage = "Parsing exception: " + std::string(e.what());
		return false;
	} catch (...) {
		errorMessage = "Unknown parsing error";
		return false;
	}
}
