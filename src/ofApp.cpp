#include "ofApp.h"

void ofApp::setup() {
	ofSetWindowTitle("Country Facts Explorer");
	ofSetFrameRate(60);
	ofSetWindowShape(1000, 700);

	// Initialize fonts
	bool titleLoaded = titleFont.load("verdana.ttf", 24);
	bool regularLoaded = regularFont.load("verdana.ttf", 16);
	bool smallLoaded = smallFont.load("verdana.ttf", 12);

	// Fallback to bitmap font if TTF fails
	if (!titleLoaded || !regularLoaded || !smallLoaded) {
		ofLogWarning() << "Could not load font files. Using bitmap font.";
	}

	// Initialize colors
	backgroundColor = ofColor(25, 25, 40);
	textColor = ofColor(220, 220, 245);
	highlightColor = ofColor(0, 150, 255);

	// Initialize state
	userInput = "United Kingdom";
	displayMessage = "Enter a country name and press Enter or click Search";
	showResults = false;
	searching = false;
}

void ofApp::update() {
	// Check for async fetch completion
	if (searching) {
		if (fetcher.hasData()) {
			currentCountry = fetcher.getCountryData();
			showResults = true;
			displayMessage.clear();
			searching = false;
			ofLogNotice() << "ofApp: Country data received and applied.";
		} else if (!fetcher.getErrorMessage().empty()) {
			displayMessage = fetcher.getErrorMessage();
			showResults = false;
			searching = false;
			ofLogError() << "ofApp: Error from fetcher: " << displayMessage;
		}
	}
}

void ofApp::draw() {
	ofBackground(backgroundColor);
	ofSetColor(textColor);

	// Draw title
	titleFont.drawString("Country Facts Explorer", 30, 50);
	smallFont.drawString("Powered by REST Countries API", 30, 80);

	// Draw UI
	drawUI();

	// Draw results or error
	if (showResults) {
		drawResults();
	} else if (!displayMessage.empty()) {
		drawError();
	}
}

void ofApp::drawUI() {
	int x = 30;
	int y = 120;

	// Draw input label
	regularFont.drawString("Country Name:", x, y);
	y += 30;

	// Draw input box
	ofSetColor(60, 60, 80);
	ofDrawRectangle(x, y, 250, 35);
	ofSetColor(textColor);
	ofNoFill();
	ofDrawRectangle(x, y, 250, 35);

	// Draw text in input box
	regularFont.drawString(userInput, x + 10, y + 25);
	y += 50;

	// Draw Search button
	searchButtonRect.set(x, y, 150, 40);
	ofSetColor(highlightColor);
	ofDrawRectangle(searchButtonRect);
	ofSetColor(255, 255, 255); 
	regularFont.drawString("Search", x + 40, y + 28);
	ofSetColor(textColor);

	// Draw Clear button
	clearButtonRect.set(x + 170, y, 100, 40);
	ofSetColor(100, 100, 120);
	ofDrawRectangle(clearButtonRect);
	ofSetColor(255, 255, 255); 
	regularFont.drawString("Clear", x + 190, y + 28);
	ofSetColor(textColor);
}

void ofApp::drawResults() {
	int x = 320;
	int y = 150;
	int lineHeight = 28;

	ofSetColor(highlightColor);
	regularFont.drawString("Country Information:", x, y);
	y += 35;

	ofSetColor(textColor);

	// Draw country name
	regularFont.drawString("Country: " + currentCountry.name, x, y);
	y += lineHeight;

	// Draw capital
	regularFont.drawString("Capital: " + currentCountry.capital, x, y);
	y += lineHeight;

	// Draw population (formatted with commas)
	std::string populationStr = "Population: " + std::to_string(currentCountry.population);
	regularFont.drawString(populationStr, x, y);
	y += lineHeight;

	// Draw region
	regularFont.drawString("Region: " + currentCountry.region, x, y);
	y += lineHeight;

	// Draw currencies
	if (!currentCountry.currencies.empty()) {
		std::string currenciesStr = "Currencies: ";
		for (size_t i = 0; i < currentCountry.currencies.size(); i++) {
			currenciesStr += currentCountry.currencies[i];
			if (i < currentCountry.currencies.size() - 1) currenciesStr += ", ";
		}
		regularFont.drawString(currenciesStr, x, y);
		y += lineHeight;
	}

	// Draw languages
	if (!currentCountry.languages.empty()) {
		std::string languagesStr = "Languages: ";
		for (size_t i = 0; i < currentCountry.languages.size(); i++) {
			languagesStr += currentCountry.languages[i];
			if (i < currentCountry.languages.size() - 1) languagesStr += ", ";
		}
		regularFont.drawString(languagesStr, x, y);
		y += lineHeight;
	}
}

void ofApp::drawError() {
	int x = 320;
	int y = 150;

	ofSetColor(255, 100, 100); // Red for errors
	regularFont.drawString(displayMessage, x, y);

	y += 50;
	ofSetColor(textColor);
	smallFont.drawString("Try: United Kingdom, France, Japan, Brazil, Australia, Germany, Spain", x, y);
}

void ofApp::keyPressed(int key) {
	if (key == OF_KEY_RETURN) {
		searchCountry();
	} else if (key == OF_KEY_BACKSPACE && !userInput.empty()) {
		userInput = userInput.substr(0, userInput.length() - 1);
	} else if (key >= 32 && key < 127) {
		userInput += (char)key;
	}
}

void ofApp::mousePressed(int x, int y, int button) {
	if (searchButtonRect.inside(x, y)) {
		searchCountry();
	}

	if (clearButtonRect.inside(x, y)) {
		userInput = "";
		showResults = false;
		displayMessage = "Cleared";
	}
}

void ofApp::searchCountry() {
	if (userInput.empty()) {
		displayMessage = "Please enter a country name";
		showResults = false;
		return;
	}

	displayMessage = "Searching for " + userInput + "...";
	showResults = false;
	searching = true;

	// Call API (async). Result will be handled in update().
	ofLogNotice() << "ofApp: Searching for country: " << userInput;
	fetcher.fetchCountryData(userInput);
	// keep searching==true until update() observes completion
}
