#include <vector>

#define NUM_EVENTS 30
#define EVENT_LEFT_SEEN_FOOD	0
#define EVENT_TOP_SEEN_FOOD		1
#define EVENT_RIGHT_SEEN_FOOD	2
#define EVENT_BOTTOM_SEEN_FOOD	3
#define EVENT_NONE_SEEN_FOOD	4
#define EVENT_LEFT_SEEN_CREATURE_EQUAL		5
#define EVENT_TOP_SEEN_CREATURE_EQUAL		6
#define EVENT_RIGHT_SEEN_CREATURE_EQUAL		7
#define EVENT_BOTTOM_SEEN_CREATURE_EQUAL	8
#define EVENT_NONE_SEEN_CREATURE_EQUAL		9
#define EVENT_LEFT_SEEN_CREATURE_BIGGER		10
#define EVENT_TOP_SEEN_CREATURE_BIGGER		11
#define EVENT_RIGHT_SEEN_CREATURE_BIGGER	12
#define EVENT_BOTTOM_SEEN_CREATURE_BIGGER	13
#define EVENT_NONE_SEEN_CREATURE_BIGGER		14
#define EVENT_LEFT_SEEN_CREATURE_SMALLER	15
#define EVENT_TOP_SEEN_CREATURE_SMALLER		16
#define EVENT_RIGHT_SEEN_CREATURE_SMALLER	17
#define EVENT_BOTTOM_SEEN_CREATURE_SMALLER	18
#define EVENT_NONE_SEEN_CREATURE_SMALLER	19
#define EVENT_LEFT_SEEN_CREATURE_MATEABLE	20
#define EVENT_TOP_SEEN_CREATURE_MATEABLE	21
#define EVENT_RIGHT_SEEN_CREATURE_MATEABLE	22
#define EVENT_BOTTOM_SEEN_CREATURE_MATEABLE	23
#define EVENT_NONE_SEEN_CREATURE_MATEABLE	24
#define EVENT_LEFT_SEEN_EDGE_OF_SCREEN		25
#define EVENT_TOP_SEEN_EDGE_OF_SCREEN		26
#define EVENT_RIGHT_SEEN_EDGE_OF_SCREEN		27
#define EVENT_BOTTOM_SEEN_EDGE_OF_SCREEN	28
#define EVENT_NOTHING_HAPPENED 29

#define NUM_ACTIONS 8
#define ACTION_DO_NOTHING	0
#define ACTION_MOVE 		1
#define ACTION_TURN_LEFT	2
#define ACTION_TURN_RIGHT	3
#define ACTION_FACE_LEFT	4
#define ACTION_FACE_UP		5
#define ACTION_FACE_RIGHT	6
#define ACTION_FACE_DOWN	7

#define DIRECTION_LEFT		0
#define DIRECTION_UP		1
#define DIRECTION_RIGHT		2
#define DIRECTION_DOWN		3

class Creature {
	public:
		int bodySize;
		int lifetime;
		int maxLifetime;
		int food;
		int maxFood;
		int xpos;
		int ypos;
		int timeLasted;
		bool dead;
		int gender;
		bool fighting;
		std::vector<std::vector<int> > genome;
		bool events[NUM_EVENTS];
		int cooldown;
		int direction;
		int step;
		int maxStep;

		//seeing radius 40

		/*
			POSSIBLE EVENTS:
				00 Left Seen	food
				01 Top Seen		food
				02 Right Seen	food
				03 Bottom Seen	food
				04 None Seen	food
				05 Left Seen	creature equal
				06 Top Seen		creature equal
				07 Right Seen	creature equal
				08 Bottom Seen	creature equal
				09 None Seen	creature equal
				10 Left Seen	creature bigger
				11 Top Seen		creature bigger
				12 Right Seen	creature bigger
				13 Bottom Seen	creature bigger
				14 None Seen	creature bigger
				15 Left Seen	creature smaller
				16 Top Seen		creature smaller
				17 Right Seen	creature smaller
				18 Bottom Seen	creature smaller
				19 None Seen	creature smaller
				20 Left Seen	creature opposite
				21 Top Seen		creature opposite
				22 Right Seen	creature opposite
				23 Bottom Seen	creature opposite
				24 None Seen	creature opposite
				25 Left Seen	edge of screen (beside) 
				26 Top Seen		edge of screen (beside) 
				27 Right Seen	edge of screen (beside) 
				28 Bottom Seen	edge of screen (beside) 
				29 See Nothing
		*/

		/*
			POSSIBLE ACTIONS:
				0 Do Nothing
				1 Move left
				2 Move up
				3 Move right
				4 Move down
		*/

		Creature(int);
		~Creature();
		int Tick();
		void Kill();
		void Action(int);
};

Creature::Creature(int bs) {
	lifetime = 1000; // base
	food = 300;
	timeLasted = 0;
	dead = false;
	gender = floor(rand() % 2); // 0 - male, 1 - female
	fighting = false;
	cooldown = 0;
	direction = 0;
	step = 0;

	xpos = floor(rand() % 200);
	ypos = floor(rand() % 200);

	bodySize = bs;
	if(bodySize < 2)
		bodySize = 2;

	lifetime += 50*bodySize;
	food += 15*bodySize;

	maxLifetime = lifetime;
	maxFood = food;

	maxStep = 3;

	//temp genome
	for(int i = 0; i <= NUM_EVENTS-1; i++) {
		std::vector<int> actions;
		for(int z = 0; z < 4; z++) {
			int action = floor(rand() % NUM_ACTIONS);
			actions.push_back(action);
		}
		genome.push_back(actions);
		events[i] = false;
	}
}
Creature::~Creature() {

}

int Creature::Tick() {
	//We have to make sure that Tick() is called after events have been updated...
	
	if(!dead) {
		food -= 2;

		if(lifetime < 0) {
			lifetime = 0;
		}
		timeLasted++;
		step++;
		if(step > maxStep) {
			step = 0;
		}

		//lifetime -= bodySize;

		if(cooldown > 0) {
			cooldown--;
		}

		// cout << "cooldown " << cooldown << endl;

		if(xpos > 180) {
			events[EVENT_RIGHT_SEEN_EDGE_OF_SCREEN] = true;
		}
		else if(xpos < 20) {
			events[EVENT_LEFT_SEEN_EDGE_OF_SCREEN] = true;
		}

		if(ypos > 180) {
			events[EVENT_BOTTOM_SEEN_EDGE_OF_SCREEN] = true;
		}
		else if(ypos < 20) {
			events[EVENT_TOP_SEEN_EDGE_OF_SCREEN] = true;
		}

		events[EVENT_NOTHING_HAPPENED] = true;
		for(int z = 0; z < NUM_EVENTS-1; z++) {
			if(events[z]) {
				events[EVENT_NOTHING_HAPPENED] = false; // events[z] is true so something happened
			}
		}

		for(int i = 0; i < NUM_EVENTS-1; i++) { // last event should always be "nothing happened"
			if(events[i]) {
				Action(genome[i][step]); // this has been ided by valgrind: invalid read size 4
				events[i] = false;
			}
		}

		if(!fighting && events[EVENT_NOTHING_HAPPENED]) {
			Action(genome[EVENT_NOTHING_HAPPENED][step]);
		}

		if(food < 0)
			food = 0;
		if(food > maxFood)
			food = maxFood;

		if(xpos > 200-bodySize) {
			xpos = 200-bodySize;
		}
		else if(xpos < 0) {
			xpos = 0;
		}

		if(ypos > 200-bodySize) {
			ypos = 200-bodySize;
		}
		else if(ypos < 0) {
			ypos = 0;
		}

		if(food == 0 || lifetime == 0 || lifetime >= 15000) {
			Kill();
		}
	}

	return timeLasted;
}
void Creature::Action(int action) {
	switch(action) {
		case ACTION_DO_NOTHING:
			break;
		case ACTION_MOVE:
			switch(direction) {
				case DIRECTION_LEFT:
					xpos -= 1;
					food -= bodySize;///2;
					break;
				case DIRECTION_UP:
					ypos -= 1;
					food -= bodySize;///2;
					break;
				case DIRECTION_RIGHT:
					xpos += 1;
					food -= bodySize;///2;
					break;
				case DIRECTION_DOWN:
					ypos += 1;
					food -= bodySize;///2;
					break;
			}
			break;
		case ACTION_TURN_LEFT:
			direction--;
			if(direction > DIRECTION_DOWN) { // can probably be removed
				direction = DIRECTION_LEFT;
			}
			if(direction < DIRECTION_LEFT) {
				direction = DIRECTION_DOWN;
			}
			break;
		case ACTION_TURN_RIGHT:
			direction++;
			if(direction > DIRECTION_DOWN) {
				direction = DIRECTION_LEFT;
			}
			if(direction < DIRECTION_LEFT) { // can probably be removed
				direction = DIRECTION_DOWN;
			}
			break;
		case ACTION_FACE_LEFT:
			direction = DIRECTION_LEFT;
			break;
		case ACTION_FACE_UP:
			direction = DIRECTION_UP;
			break;
		case ACTION_FACE_RIGHT:
			direction = DIRECTION_RIGHT;
			break;
		case ACTION_FACE_DOWN:
			direction = DIRECTION_DOWN;
			break;
	}
}
void Creature::Kill() {
	dead = true;
}