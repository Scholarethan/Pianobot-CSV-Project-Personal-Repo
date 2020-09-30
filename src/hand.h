#ifndef   __Hand__
#define   __Hand__
#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>

using std::vector;

// State to store each instance of a key-press changes
struct state
{
	int time;
	bool fingers[16];
	int updatedFing;
	bool onOff;
};

// Hand positions struct to store the next ahnd position
struct handPos
{
	int time;
	int pos;
	vector<state> states;
};

using namespace std;

// Hand Class
class Hand
{
private:
	int posNum;
	int stateNum;

public:
	// Vector to store all hand positions
	vector<handPos> positions;

	// Function details can be found in .cpp file
	Hand();
	~Hand();

	void addHandPos(int time, int position);
	void addState(int time, int finger, bool onOff);
	bool canMove();
	int getHandPos();
	state getState();
	int handMoveDist(int note);
	int findFinger(int note);
	
};
#endif