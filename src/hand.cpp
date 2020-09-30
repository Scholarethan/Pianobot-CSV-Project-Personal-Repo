#include "Hand.h"

using namespace std;

// Constructor to create a hand position with no fingers pressed
Hand::Hand()
{
	// Initialize position and state indices to -1 as they are empty
	posNum = -1;
	stateNum = -1;
}

// Empty destructor
Hand::~Hand() {}

// Adding a hand position using given time and position
void Hand::addHandPos(int time, int position)
{
	// Create a new hand position
	handPos newPosition;
	newPosition.time = time;
	newPosition.pos = position;

	// Append created hand position
	positions.push_back(newPosition);

	// Reset state number and increment position index
	stateNum = -1;
	posNum++;
}

// Adding a fingers state using given time and state
void Hand::addState(int time, int finger, bool onOff)
{
	// Create new state
	state newState;

	// Iteratively copy over the finger state
	newState.time = time;
	for (int i = 0; i < 16; i++)
	{
		if (i != finger)
		{
			if (stateNum == -1)
			{
				newState.fingers[i] = 0;
			}
			else
			{
				bool prevState = positions[posNum].states[stateNum].fingers[i];
				newState.fingers[i] = prevState;
			}
		}
		else
		{
			newState.fingers[i] = onOff;
		}
	}
	newState.updatedFing = finger;
	newState.onOff = onOff;

	// Append created state
	positions[posNum].states.push_back(newState);

	// Increment state index
	stateNum++;
}

// Check if any finger is pressed
bool Hand::canMove()
{
	// Get current state
	state currState = positions[posNum].states[stateNum];

	// Check if any fingers are pressed down
	for (int i = 0; i < 16; i++)
	{
		if (currState.fingers[i] == 1)
		{
			return false;
		}
	}
	return true;
}

// Return current hand position
int Hand::getHandPos()
{
	return positions[posNum].pos;
}

// Return current finger state
state Hand::getState()
{
	return positions[posNum].states[stateNum];
}

// Checks how far do I need to move for a note in white keys
// Param: note is the midi note value
// Uses the current position of the hand
// Returns:
//	<0 	Move left
//	0 	In current range of fingers
//	>0 	Move right 
int Hand::handMoveDist(int note)
{
	// Get current position
	int currentPos = positions[posNum].pos;

	// Get relative position (removing octaves) - thumb
	int relativePos = currentPos % 12;

	// Calculate # octaves
	int octaves = floor(((double)note - (double)currentPos) / 12.0);

	// Calculate relative note position (removing octaves)
	int relativeNotePos = note % 12;

	// Used to store # white keys movements needed
	int toMove;

	// Used to determine number of white keys to move
	// First index is hand's relative white key position
	// Second index is the finger's relative position
	int moveRef[7][12] = 
	{
		// Formula is equivalent to:
		// ([firstRow] - (relativehandPos)) % 7

		// Semitones from C natural
		//0  1  2  3  4  5  6  7  8  9 10 11
		{ 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6},
		{ 6, 6, 0, 0, 1, 2, 2, 3, 3, 4, 4, 5},
		{ 5, 5, 6, 6, 0, 1, 1, 2, 2, 3, 3, 4},
		{ 4, 4, 5, 5, 6, 0, 0, 1, 1, 2, 2, 3},
		{ 3, 3, 4, 4, 5, 6, 6, 0, 0, 1, 1, 2},
		{ 2, 2, 3, 3, 4, 5, 5, 6, 6, 0, 0, 1},
		{ 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6, 0}
	};

	// Used to figure out hand position index for moveReference
	int relativeHandPos[12] = {0, -1, 1, -1, 2, 3, -1, 4, -1, 5, -1, 6};
	
	// Calculate relative hand position of white keys
	int handPosIdx = relativeHandPos[relativePos];

	// Find the number of white keys to move
	if (octaves == 0) // In range of current hand
	{
		return 0; // Don't need to move
	}
	else if (octaves > 0) // Move right
	{
		toMove = moveRef[handPosIdx][relativeNotePos];
		
		// Subtract 1 to account for hand span
		toMove += (octaves - 1) * 7;
	}
	else // Move left
	{
		toMove = moveRef[handPosIdx][relativeNotePos];
		toMove += octaves * 7;
	}
	
	return toMove;
}

// Finds which finger of the in-range note (from 0 to 15)
// Param: note is the midi note value
// Robot: 8 white fingers and 8 black fingers
// Returns the finger that is on the note
// White fingers are 0 - 7
// Black fingers are 8 - 15
int Hand::findFinger(int note)
{
	// If the note isn't in range of the hand span
	if (handMoveDist(note) != 0)
	{
		return -1;
	}

	// Get relative position
	int currentPos = positions[posNum].pos;

	// Get relative position (removing octaves)
	int relativePos = currentPos % 12;

	// Calculate relative note position (removing octaves)
	int relativeNotePos = note % 12;

	// Calculate difference in position
	int diffPos = note - currentPos;

	// Used to figure out if note is white or black
	bool isBlack[12] = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};

	// First index is hand's relative white key position
	// Second index is the finger's relative position
	int moveRef[7][14] = 
	{
		{ 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6, 7, 7},
		{ 0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7},
		{ 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, -1},
		{ 0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 7, 7},
		{ 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7},
		{ 0, 0, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7},
		{ 0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6, 7, -1}
	};

	// Used to figure out hand position index for moveReference
	int relativeHandPos[12] = {0, -1, 1, -1, 2, 3, -1, 4, -1, 5, -1, 6};

	// Calculate relative hand position of white keys
	int handPosIdx = relativeHandPos[relativePos];

	// Calculate finger offset value
	int finger = moveRef[handPosIdx][diffPos];

	// Adjust value for black key after considering offset
	if (isBlack[relativeNotePos] == true)
	{
		finger += 8;
	}

	return finger;
}