#include "Hand.h"
#include "Song.h"

using namespace std;
/* Several objectives and milestones:
 * 1. Converts to the correct format
 * 2. Adds in proper stepper controls
 * 3. Add optimization for hand paths (later)
 */

 // In the Song and Hand classes, break down tasks into smaller functions
 // Add as many as necessary

int main(void)
{
	// Create the song and import the .csv data
	Song song;
	song.importSong();

	// Flag to indicate if completed
	bool done = false;

	// While there is still data to add
	while (!done)
	{
		done = song.addLine();
	}

	// Export code in arduino format
	song.exportArduino();

	cout << "Done!" << endl;

	return 0;
}

/*
Removing a note from the txt file: delete the pair (on/off) for that note. Make sure on is first.
Modifying a note from the txt file: change the midi number for the pair (on/off)
*/
