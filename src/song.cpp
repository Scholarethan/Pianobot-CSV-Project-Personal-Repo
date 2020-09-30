#include "Song.h"

using namespace std;

Song::Song() 
{
	currLine = 0;
}

Song::~Song() {}

// Import song data into the vectors
void Song::importSong()
{
 	// Open up file
	inFile.open("Twinkle.txt", ios::in);
	if (!inFile)
	{
		cout << "Unable to open file";
		exit(1); // Terminate with error
	}

	// Buffer to store input values
	string buffer;

	// Reset row for a new song
	row = 0;

	// Flag to determine which column it is during input
	int flag = 0;

	// Loop through to save all the values
	while (!inFile.eof())
	{
		if (flag == 0)
		{
			// Get data from CSV file
			getline(inFile, buffer, ',');

			// Append to the vector
			if (!buffer.empty())
				time.push_back(stoi(buffer));
			
			// For debugging
			//cout << time[row] << ", ";

			// Increment counter to onOff
			flag++;
		}
		else if (flag == 1)
		{
			// Get data from CSV file
			getline(inFile, buffer, ',');

			// Append to the vector
			if (!buffer.empty())
				onOff.push_back(stoi(buffer));

			// For debugging
			//cout << onOff[row] << ", ";

			// Increment counter to note
			flag++;
		}
		else
		{
			// Get data from CSV file
			getline(inFile, buffer, '\n');

			// Append to the vector
			if (!buffer.empty())
				note.push_back(stoi(buffer));

			// For debugging
			//cout << note[row] << "\n";

			// Increment for one row of data
			row++;

			// Reset counter to time
			flag = 0;
		}
	}
}

// Export current objects into an arduino .txt file
// Output Formatting:
// E.g. (Time)_(Hand)(OnOff)
// 		(Time)_(LH)(LHDirection)(LHDistance)
// 
// H: right hand			e.g. H1_5 move right 5 keys
// h: left hand				e.g. h0_2 move left 2 keys
// F: right hand fingers	e.g. F1_15 press the 15th fingers
// f: left hand fingers		e.g. f0_4 release the 4th finger
// t: time					e.g. t1424 1424 milliseconds from the start
void Song::exportArduino()
{
	// Open/create a new file to store the output for Arduino
	outFile.open("pianocontrol.txt", ios::out);
	if (!outFile)
	{
		cout << "Unable to open file";
		exit(1); // Terminate with error
	}

	// Initialize variables for the loop
    string outLine = "";

	// Store LH and RH times
  	int timeLH = 0;
	int timeRH = 0;

	// Indices to iterate through hand positions and states
  	int idxLH = 0;
  	int stateLH = 0;
  	int idxRH = 0;
  	int stateRH = 0;

	// Flag to denote new hand position
	bool newHandPosLH = true;
	bool newHandPosRH = true;

	// Value to store calculated difference in hand position (white keys width)
	int posDiff;

	// Loop until complete
	while (idxLH < LH.positions.size() || idxRH < RH.positions.size())
	{
		// Get Left hand time, hand position, then fingers
		if (newHandPosLH)
		{
			timeLH = LH.positions[idxLH].time;
		}
		else
		{
			timeLH = LH.positions[idxLH].states[stateLH].time;
		}

		// Get Right hand time, hand position, then fingers
        if (newHandPosRH)
		{
			timeRH = RH.positions[idxRH].time;
		}
		else
		{
			timeLH = LH.positions[idxLH].states[stateLH].time;
		}
        
		// LH had smaller time
        if (timeLH <= timeRH) 
		{
			// Add the timestamp
			outLine += "t" + to_string(timeLH) + "_";

			// Add a hand movement if hand position changed
			if (newHandPosLH)
			{
				// Calculate the position difference
				// If beginning of song, move relative to initial positions
				if (idxLH == 0)
				{
					stateLH == LH.positions[idxLH].pos - initPosLH;
				}
				else
				{
					posDiff = LH.positions[idxLH].pos - LH.positions[idxLH - 1].pos;
				}

				// First number depends on direction of motion
				if (posDiff > 0)
				{
					outLine += "h1" + to_string(posDiff);
				}
				else
				{
					outLine += "h0" + to_string(-posDiff);
				}
			}
			else
			{
				// Add the finger changes and states
				state curr = LH.positions[idxLH].states[stateLH];
				outLine += "f";
				outLine += to_string(curr.onOff) + "_";
				outLine += to_string(curr.updatedFing);
			}
          stateLH++;
		}

		//RH had smaller time
      	else 
		{
			// Add the timestamp
			outLine += "t" + to_string(timeRH) + "_";

			// Add a hand movement if hand position changed
			if (newHandPosRH)
			{
				// Calculate the position difference
				// If beginning of song, move relative to initial positions
				if (idxRH == 0)
				{
					stateRH == RH.positions[idxRH].pos - initPosRH;
				}
				else
				{
					posDiff = RH.positions[idxRH].pos - RH.positions[idxRH - 1].pos;
				}
				
				// First number depends on direction of motion
				if (posDiff > 0)
				{
					outLine += "H1" + to_string(posDiff);
				}
				else
				{
					outLine += "H0" + to_string(-posDiff);
				}
			}
			else
			{
				// Add the finger changes and states
				state curr = RH.positions[idxRH].states[stateRH];
				outLine += "F";
				outLine += to_string(curr.onOff) + "_";
				outLine += to_string(curr.updatedFing);
			}
         	stateRH++;
		}
      		
  		// Output to file
		outFile << outLine << endl;

		// Reset to the next hand position after the previous is iterated through
      	if (stateLH == LH.positions[idxLH].states.size())
		{
			// Increment Hand position index, reset state, and set flag
			idxLH++;
			stateLH = 0;
			newHandPosLH = true;
		}
		else if (stateRH == RH.positions[idxRH].states.size())
		{
			// Increment Hand position index, reset state, and set flag
			idxRH++;
			stateRH = 0;
			newHandPosRH = true;
		}
	}
}

// Add the next line of data from time, onOff, and note to the Hand classes
// Note: Some redundancies and copy-pastes could be removed for clarity
// Dependent on position/path optimization, take a look at that after when we get a chance
bool Song::addLine()
{
	int moveDistLH = LH.handMoveDist(note[currLine]);
	int moveDistRH = RH.handMoveDist(note[currLine]);
	int currTime = time[currLine];
	bool currOnOff = onOff[currLine];
	//bool moveRH; // Indicate if RH or LH needs to move
	bool keyRH; // Indicate if RH or LH adds a state (false = LH, true = RH)
  	int moveHands = 0; //indicate if RH or LH needs to move to reach the next note (0 = Neither, 1 = LH, 2 = RH)
  	

	// If first line, need to initialize and positions
	if (currLine == 0)
	{
		// Initialize positions such that the first note will be within one
		// of the hands' span
		initializeHandPos();
		if (moveDistLH == 0)
		{
			keyRH = false;
		}
		else
		{
			keyRH = true;
		}
	}
  
	// Otherwise, hand might need to move
	else
	{
		// If the note is within range of one of the hands
		if (moveDistLH == 0 || moveDistRH == 0)
		{
			if (moveDistLH == 0)
			{
				keyRH = false;
			}
			else
			{
				keyRH = true;
			}
		}
		// If the note is out of range of both hands
		// Move nearest hand if it is movable
		// 3 Cases:
		// 	Left of LH
		// 	Right of RH
		// 	Between both hands
		// Otherwise error (return -1)
		else
		{
			// Case 1: Left of LH
			if (moveDistLH < 0)
			{
				if (LH.canMove() == false)
				{
					cout << "Note left of LH but is already pressing a key"; // Position optimization issue
					return -1;
				}
				moveHands = 1;
				keyRH = false;
			}

			// Case 2: Right of RH
			else if (moveDistRH > 0)
			{
				if (RH.canMove() == false)
				{
					cout << "Note right of RH but is already pressing a key";
					return -1;
				}
				moveHands = 2;
				keyRH = true;
			}

			// Case 3: Between both hands
			else
			{
				// Neither hand can move
				if (RH.canMove() == false && LH.canMove() == false)
				{
					cout << "Neither hand can move, pressing a key";
					return -1;
				}

				// Only LH can move
				else if (RH.canMove() == false)
				{
					moveHands = 1;
					keyRH = false;
				}

				// Only RH can move
				else if (LH.canMove() == false)
				{
					moveHands = 2;
					keyRH = true;
				}

				// Both hands can move
				else
				{
					// If LH is further away or equal distance away
					// Priority to the hand playing the melody
					if (fabs(moveDistLH) >= fabs(moveDistRH))
					{
						moveHands = 2;
						keyRH = true;
					}

					// If RH is further away
					else
					{
						moveHands = 1;
						keyRH = false;
					}
				}
			}
		}
	}

	// First move the hand if needed
	if (moveHands == 2)
	{
		// Obtain new needed hand position
		int newPos = RH.getHandPos() + moveDistRH;
		RH.addHandPos(currTime, newPos);
	}
	else if (moveHands == 1)
	{
		// Obtain new needed hand position
		int newPos = LH.getHandPos() + moveDistLH;
		LH.addHandPos(currTime, newPos);
	}

	// Then, add the finger state
	if (keyRH)
	{
		RH.addState(currTime, RH.findFinger(note[currLine]), currOnOff);
	}
	else
	{
		LH.addState(currTime, LH.findFinger(note[currLine]), currOnOff);
	}
	
	return true;
}

// Find initial starting hand positions using the note vector and buffer
void Song::initializeHandPos()
{
	// Initialize variables to store initial hand positions
	int RHstart;
	int LHstart;

	// Flag to determine if the hand positions were set
	int flag = false;

	// Loop through all notes to find one that is far away from the first
	// Minimum distance set by handBuffer
	for (int i = 1; i < note.size(); i++) 
	{
		// Check distance from first note
		if (abs(note[i] - note.front()) > handBuffer) 
		{
			// Put the rightmost note on the RH, and leftmost on LH
			if (note[i] > note.front())
			{
				LHstart = note.front();
				RHstart = note[i];
			}
			else
			{
				LHstart = note[i];
				RHstart = note.front();
			}

			// Hand positions have been set
			flag = true;
			break;
       	}
	}

	// If hand positions were set, use to initial hands
	if (flag)
	{
		RH.addHandPos(0, RHstart);
		LH.addHandPos(0, LHstart);
	}
	else
	{
		cout << "No note was >" << handBuffer << " semitones away." << endl;
		cout << "Hand positions were not set." << endl;
	}
}