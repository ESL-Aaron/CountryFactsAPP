#pragma once
#include "CountryFetcher.h"
#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void mousePressed(int x, int y, int button);

private:
	CountryFetcher fetcher;
	CountryData currentCountry;

	// UI state
	std::string userInput;
	std::string displayMessage;
	bool showResults;
	bool searching;

	// Fonts
	ofTrueTypeFont titleFont;
	ofTrueTypeFont regularFont;
	ofTrueTypeFont smallFont;

	// Colors
	ofColor backgroundColor;
	ofColor textColor;
	ofColor highlightColor;

	// Button rectangles for click detection
	ofRectangle searchButtonRect;
	ofRectangle clearButtonRect;

	// Custom drawing helpers
	void drawResults();
	void drawError();
	void drawUI();
	void searchCountry();
};
