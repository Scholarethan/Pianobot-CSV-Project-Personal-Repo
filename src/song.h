#ifndef   __Song__
#define   __Song__
#include <iostream>
#include <string>
#include <fstream>
#include "Hand.h"

// May instead use a function to determine where the hands start
const int RHstart = 60; // Start on C4 for right hand
const int LHstart = 36; // Start on C2 for left hand

const int octave = 12; // Set #semitones
const int limitL = 48; // Left limit
const int limitR = 108; // Right limit

using namespace std;

class Song
{
private:
	// Left and right hand variables
	Hand LH;
	Hand RH;

	// Vectors to store data from .csv file
	vector<int> time;
	vector<bool> onOff;
	vector<int> note;

	// Input/ouput file streams
	fstream outFile;
	fstream inFile;

	// Total rows of data
	int row;

	// Current line of data
	int currLine;

	// Buffer to indicate distance between initialized hand positions
	// Note: Must be >12 to ensure hands do not physically collide
	const int handBuffer = 12;

	// Initial hand positions
	const int initPosLH = 48;
	const int initPosRH = 72;

public:
	// Function details can be found in .cpp file
	Song();
	~Song();

	void importSong();
	void exportArduino();
	bool addLine();
	void initializeHandPos();
	int findFing(int handPos, int note);
};
#endif