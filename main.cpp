// (c) David Lougheed 2013-14.

#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>
#include <cmath>

#include "SDL.h"
#include "SDL_opengl.h"

#include "Util.h"
#include "Game.h"
#include "Creature.h"
#include "Food.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define SCREEN_BPP 24
#define FRAMES_PER_SECOND 120

using namespace std;
using std::string;

SDL_Event event;

float life_color[3] = {255.0f, 0.0f, 0.0f};
float food_color[3] = {255.0f, 255.0f, 0.0f};
float gender_color[2][3] = {{255.0f, 0.0f, 255.0f}, {128.0f, 255.0f, 128.0f}};

vector<Creature> creatures;
vector<Food> food;

bool quit = false;
vector<vector<int> > breedCreatures;
vector<int> killCreatures;
vector<int> killFood;

time_t totalTime;

int restart_count = 0;

vector<vector<int> > bestGenome;
vector<int> bestLifetime;
int generation = 0;
bool last15 = false;
int avgGeneration = 0;

int start_ticks = 0;

int main(int argc, char* argv[]) {
	if(!init("Evolution", SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP))
		return 1;

	srand(time(NULL));

	bestLifetime.push_back(0);

	for(int i = 0; i < 10000; i++) {
		Food gen_food;
		food.push_back(gen_food);
	}

	for(int z = 0; z < 200; z++) {
		Creature gen_creature(2);
		creatures.push_back(gen_creature);
	}

	console_log("Starting loop...");
	totalTime = time(NULL);

	while(!quit) {
		start_ticks = SDL_GetTicks();

		if(creatures.size() <= 15 && !last15) {
			last15 = true;
			cout << "Last 15 creatures of generation " << generation+1 << endl;
		}
		bool allDead = true;
		for(int i = 0; i < creatures.size(); i++) {
			if(!creatures[i].dead)
				allDead = false;
		}
		if(allDead && last15) {

			generation++;
			last15 = false;

			int best = 0;
			int bestCreature = 0;
			int average = 0;
			for(int i = 0; i < creatures.size(); i++) {
				if(creatures[i].timeLasted > best) {
					best = creatures[i].timeLasted;
					bestCreature = i;
				}
				average += creatures[i].timeLasted;
			}
			avgGeneration += average;
			average = average / creatures.size();
			avgGeneration = avgGeneration / 200;

			cout << "Generation " << generation << ": Best Lifetime: " << creatures[bestCreature].timeLasted << " Average Lifetime: " << average << " Average for All Generation: " << avgGeneration << endl;
			avgGeneration = 0;

			bestGenome = creatures[bestCreature].genome;

			bestLifetime.push_back(creatures[bestCreature].timeLasted);

			food.clear();

			for(int i = 0; i < 10000; i++) {
				Food gen_food;
				food.push_back(gen_food);
			}

			vector<Creature> new_creatures;

			for(int z = 0; z < 200; z++) {
				vector<vector<int> > new_genome;

				int lastSize = creatures.size();
				int randCreature = rand() % lastSize;

				int mutation_chance = floor(rand() % 100);
				int bs = creatures[randCreature].bodySize;
				if(mutation_chance <= 6) {
					int randBodySize = floor(rand() % 3) - 1;
					bs += randBodySize;
					if(bs < 2) {
						bs = 2;
					}
				}
				try {
					new_genome = nomate_mutate(creatures[randCreature].genome, 4);
				}
				catch(exception& e) {
					cout << "exception in genome mixing: " << e.what() << endl;
					throw;
				}

				Creature gen_creature(bs);

				gen_creature.genome = new_genome;

				new_creatures.push_back(gen_creature);
			}

			creatures.clear();
			creatures = new_creatures;
		}

		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				quit = true;
			}
		}
		if(keyisdown(SDLK_TAB)) {
			vector<vector<int> > genome_d = creatures[creatures.size()-1].genome;
			cout << "\n\nGENOME\n-----------------------------------" << endl;
			for(int i = 0; i < genome_d.size(); i++) {
				cout << i << ": " << flush;
				for(int z = 0; z < genome_d[i].size(); z++) {
					int action = genome_d[i][z];
					std::string actionStr;
					switch(action) {
						case 0:
							actionStr = "none";
							break;
						case 1:
							actionStr = "move";
							break;
						case 2:
							actionStr = "trnL";
							break;
						case 3:
							actionStr = "trnR";
							break;
						case 4:
							actionStr = "fceL";
							break;
						case 5:
							actionStr = "fceU";
							break;
						case 6:
							actionStr = "fceR";
							break;
						case 7:
							actionStr = "fceD";
							break;
					}
					cout << actionStr << " " << flush;
				}
				cout << endl;
			}
			cout << "-----------------------------------\n" << endl;
		}
		if(keyisdown(SDLK_SPACE)) {
			cout << endl;
			console_log("Mark");
			cout << endl;
		}

		if(food.size() < 100000) {
			for(int i = 0; i < 30; i++) {
				food.push_back(Food());
			}
		}

		const size_t zsize = creatures.size();

		for(unsigned z = 0; z < zsize; z++) {
			if(!creatures[z].dead) {
				creatures[z].Tick();

				if(!creatures[z].dead) {
					const int zx = creatures[z].xpos;
					const int zy = creatures[z].ypos;
					const int zbody = creatures[z].bodySize;
					const bool zgender = creatures[z].gender;

					for(unsigned i = 0; i < food.size(); i++) {
						float delta_x = food[i].xpos - zx;
						float delta_y =	food[i].ypos - zy;
						float length = sqrt((pow(delta_x, 2) + pow(delta_y, 2)));
						
						if(abs(length) <= 4) {
							if(food[i].xpos >= zx-1 && food[i].xpos+1 <= zx+1+creatures[z].bodySize) {
								if(food[i].ypos >= zy-1 && food[i].ypos+1 <= zy+1+zbody) {
									if(!food[i].firstflag) {
										creatures[z].food += 25;
										food[i].Kill();
										killFood.push_back(i);
									}
									else {
										food[i].xpos = 0 + floor(rand() % 200);
										food[i].ypos = 0 + floor(rand() % 200);
									}
								}
								else {
									food[i].firstflag = false;
								}
							}
							else {
								food[i].firstflag = false;
							}
							if(!food[i].dead) {
								//continue
								float angle = atan2(delta_y, delta_x) * 57.2957795131; // 180 / pi

								/*
									if 45 < angle < 135: below
									if -45 < angle < 45: right
									if 135 < angle < 180 || -180 < angle < -135: left
									if -135 < angle < -45: above
								*/

								if((angle > 135.0f && angle <= 180.0f) || (angle > -180.0f && angle <= -135.0f)) {
									creatures[z].events[0] = true;
								}
								else if(angle > -135.0f && angle <= -45.0f) {
									creatures[z].events[1] = true;
								}
								else if(angle > -45.0f && angle <= 45.0f) {
									creatures[z].events[2] = true;
								}
								else if(angle > 45.0f && angle <= 135.0f) {
									creatures[z].events[3] = true;
								}
							}
						}
						else {
							creatures[z].events[4] = true;
						}
					}

					// last step
					for(unsigned z2 = 0; z2 < zsize; z2++) {
						if(z != z2 && !creatures[z2].dead && !creatures[z].dead) { //  && !creatures[z].dead but should never happen...
							//SEEN EVENT
							int delta_x = creatures[z2].xpos - zx;
							int delta_y = creatures[z2].ypos - zy;
							float length = sqrt((pow((float)delta_x, 2) + pow((float)delta_y, 2)));
							int z_offset = 0;
							int delta_size = zbody-creatures[z2].bodySize; // if positive, z is bigger, else z2 is

							if(zgender == creatures[z2].gender) {
								if(delta_size > 0) {
									//z is bigger
									z_offset = 15;
								}
								else if(delta_size == 0) {
									//equal
									z_offset = 5;
								}
								else if(delta_size < 0) {
									//z2 is bigger
									z_offset = 10;
								}
							}
							else {
								z_offset = 20;
							}

							if(abs(length) <= 4) {
								//continue
								float angle = atan2(delta_y, delta_x) * 57.2957795131; // 180 / pi

								/*
									if 45 < angle < 135: below
									if -45 < angle < 45: right
									if 135 < angle < 180 || -180 < angle < -135: left
									if -135 < angle < -45: above
								*/

								if((angle > 135 && angle <= 180) || (angle > -180 && angle <= -135)) {
									creatures[z].events[z_offset] = true;
								}
								else if(angle > -135 && angle <= -45) {
									creatures[z].events[z_offset+1] = true;
								}
								else if(angle > -45 && angle <= 45) {
									creatures[z].events[z_offset+2] = true;
								}
								else if(angle > 45 && angle <= 135) {
									creatures[z].events[z_offset+3] = true;
								}

								if(creatures[z2].xpos*4 >= zx*4 && (creatures[z2].xpos*4)+(4*creatures[z2].bodySize) <= zx*4+(4*zbody)) {
									if(creatures[z2].ypos*4 >= zy*4 && (creatures[z2].ypos*4)+(4*creatures[z2].bodySize) <= zy*4+(4*zbody)) {
										if(zgender != creatures[z2].gender) {
											if(zbody-3 < creatures[z2].bodySize && zbody+3 > creatures[z2].bodySize) {
												if(creatures[z].cooldown <= 0 && creatures[z2].cooldown <= 0) {
													// Below was code for the mating function. RIP
													
													//cout << "mating" << endl;

													// breedCreature1 = z;
													// breedCreature2 = z2;

													// createCreatureFlag = true;

													// vector<int> breedArray;
													// breedArray.push_back(z);
													// breedArray.push_back(z2);
													// breedCreatures.push_back(breedArray);
													// creatures[z].food -= 20;
													// creatures[z2].food -= 20;

													//A feature which was never implemented...
												}
											}
										}
										else {
											if(!creatures[z].dead && !creatures[z2].dead) {
												creatures[z].lifetime -= 1;
												creatures[z2].lifetime -= 1;
												creatures[z].food -= 2;
												creatures[z2].food -= 2;
												creatures[z].fighting = true;
												creatures[z2].fighting = true;

												if(creatures[z].lifetime <= 0) {
													creatures[z].Kill();
													killCreatures.push_back(z);
												}
												if(creatures[z2].lifetime <= 0) {
													creatures[z2].Kill();
													killCreatures.push_back(z2);
												}

												if((creatures[z].dead || creatures[z2].dead) || (creatures[z].lifetime <= 0 || creatures[z2].lifetime <= 0)) {
													creatures[z].fighting = false;
													creatures[z2].fighting = false;
												}
											}
										}
									}
									else {
										creatures[z].fighting = false;
										creatures[z2].fighting = false;
									}
								}
								else {
									creatures[z].fighting = false;
									creatures[z2].fighting = false;
								}
							}
							else {
								creatures[z].events[z_offset+4] = true;
							}
						}
						else {
							if(creatures[z].dead) {
								// creatures[z].Kill();
								killCreatures.push_back(z);
							}
							if(creatures[z2].dead) {
								// creatures[z2].Kill();
								killCreatures.push_back(z2);
							}
						}
					}

					if(creatures[z].lifetime <= 0) {
						killCreatures.push_back(z);
					}
				}
				else {
					killCreatures.push_back(z);
				}
			}
		}

		// Below was code for the mating function. RIP

		// while(breedCreatures.size() > 0) {
		// 	int breedCreature1 = breedCreatures[0][0];
		// 	int breedCreature2 = breedCreatures[0][1];

		// 	int rand_bodysize_choose = floor(rand() % 100);
		// 	int body = breedCreature1;
		// 	if(rand_bodysize_choose <= 50) {
		// 		body = breedCreature2;
		// 	}
		// 	int mutation_chance = floor(rand() % 100);
		// 	int bs = creatures[body].bodySize;
		// 	if(mutation_chance <= 3) {
		// 		int randBodySize = floor(rand() % 3) - 1;
		// 		bs += randBodySize;
		// 		if(bs < 1) {
		// 			bs = 1;
		// 		}
		// 	}

		// 	Creature gen_creature(bs);
		// 	gen_creature.xpos = creatures[breedCreature1].xpos;
		// 	gen_creature.ypos = creatures[breedCreature1].ypos;

		// 	int rand_x_offset = floor(rand() % 10) - 5;
		// 	int rand_y_offset = floor(rand() % 10) - 5;

		// 	gen_creature.xpos += rand_x_offset;
		// 	gen_creature.ypos += rand_y_offset;

		// 	vector<vector<int> > new_genome;

		// 	try {
		// 		new_genome = mix_genome(creatures[breedCreature1].genome, creatures[breedCreature2].genome, 4);
		// 	}
		// 	catch(exception& e) {
		// 		cout << "exception in genome mixing: " << e.what() << endl;
		// 		throw;
		// 	}

		// 	gen_creature.genome = new_genome;

		// 	gen_creature.cooldown = 250;

		// 	try {
		// 		creatures.push_back(gen_creature);

		// 		creatures[breedCreature1].cooldown = 100;
		// 		creatures[breedCreature2].cooldown = 100;

		// 		breedCreatures.erase(breedCreatures.begin());
		// 	}
		// 	catch(exception& e) {
		// 		cout << "push back error: " << e.what() << endl;
		// 	}
		// }

		if(killCreatures.size() > 0) {
			if(!last15) {
				try {
					sort(killCreatures.begin(), killCreatures.end());
					killCreatures.erase(unique(killCreatures.begin(), killCreatures.end()), killCreatures.end());
				}
				catch(exception& e) {
					cout << "exception 1: " << e.what() << endl;
					throw;
				}
			}
		}

		int iteratorCount = 0;

		if(!last15) {
			while(killCreatures.size() > 0) {
				int creatureToKill = killCreatures[0]-iteratorCount;
				creatures[creatureToKill].Kill();

				avgGeneration += creatures[creatureToKill].timeLasted;

				try {
					creatures.erase(creatures.begin() + creatureToKill);
				}
				catch(exception& e) {
					cout << "exception 2: " << e.what() << " Vector Length: " << creatures.size() << " iteratorCount " << iteratorCount << endl;
					throw;
					quit = true;
				}
				try {
					killCreatures.erase(killCreatures.begin());
				}
				catch(exception& e) {
					cout << "exception 3: " << e.what() << endl;
					throw;
					quit = true;
				}

				iteratorCount++;
			}
		}
		else {
			for(int i = 0; i < killCreatures.size(); i++) {
				creatures[killCreatures[i]].Kill();
			}
		}

		iteratorCount = 0;

		if(killFood.size() > 0) {
			try {
				sort(killFood.begin(), killFood.end());
				killFood.erase(unique(killFood.begin(), killFood.end()), killFood.end());
			}
			catch(exception& e) {
				cout << "exception 4: " << e.what() << endl;
				throw;
				quit = true;
			}
		}

		while(killFood.size() > 0) {
			int foodToKill = killFood[0]-iteratorCount;

			food.erase(food.begin() + foodToKill);
			killFood.erase(killFood.begin());

			iteratorCount++;
		}

		clear_screen();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glTranslatef(0, 0, 0);

		for(unsigned z = 0; z < creatures.size(); z++) {
			if(!creatures[z].dead) {
				float creature_color[3] = {0.0f, 0.0f, 0.0f};
				float creature_color_fight[3] = {255.0f, 0.0f, 0.0f};

				int percentLifetime = round((float(creatures[z].lifetime)/float(creatures[z].maxLifetime))*(4.0f*float(creatures[z].bodySize)));
				int percentFood = round((float(creatures[z].food)/float(creatures[z].maxFood))*(4.0f*float(creatures[z].bodySize)));

				if(!creatures[z].fighting)
					draw_rect_color(creatures[z].xpos*4, creatures[z].ypos*4, 4*creatures[z].bodySize, 4*creatures[z].bodySize, creature_color);
				else
					draw_rect_color(creatures[z].xpos*4, creatures[z].ypos*4, 4*creatures[z].bodySize, 4*creatures[z].bodySize, creature_color_fight);
				draw_rect_color(creatures[z].xpos*4, creatures[z].ypos*4, percentLifetime, 1, life_color);
				draw_rect_color(creatures[z].xpos*4, creatures[z].ypos*4+1, percentFood, 1, food_color);
				draw_rect_color(creatures[z].xpos*4, creatures[z].ypos*4+3, 2, 2, gender_color[creatures[z].gender]);
			}
		}
		for(unsigned i = 0; i < food.size(); i++) {
			float food_color[3] = {0.0f, 1.0f, 0.0f};

			draw_rect_color(food[i].xpos*4, food[i].ypos*4, 4, 4, food_color);
		}

		glLoadIdentity();

		flip_screen();

		int time_used = SDL_GetTicks() - start_ticks;

		if(time_used < 1000/FRAMES_PER_SECOND) {
			SDL_Delay((1000/FRAMES_PER_SECOND)-(time_used+1));
		}
	}

	clean_up();

	return 0;
}