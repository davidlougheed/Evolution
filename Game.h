bool init_GL(int sw, int sh) {
	glEnable(GL_TEXTURE_2D);
	glClearColor(0, 0, 0, 0);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, sw, sh, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(glGetError() != GL_NO_ERROR) {
		return false;
	}

	return true;
}

bool init(std::string message, int sw, int sh, int sb) {
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		return false;
	}
	if(SDL_SetVideoMode(sw, sh, sb, SDL_OPENGL) == NULL) {
		return false;
	}
	if(init_GL(sw, sh) == false) {
		return false;
	}

	SDL_WM_SetCaption(message.c_str(), NULL);
	console_log("Started");

	return true;
}

void clean_up() {
	SDL_Quit();
}

void draw_rect(int x, int y, int w, int h, GLuint texture, int d) {
	glEnable(GL_TEXTURE_2D);
	glTranslatef(x, y, 0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBegin(GL_QUADS);

	glTexCoord2i(0, 0);
	glVertex3f(0, 0, d);
	
	glTexCoord2i(0, 1);
	glVertex3f(0, h, d);
	
	glTexCoord2i(1, 1);
	glVertex3f(w, h, d);
	
	glTexCoord2i(1, 0);
	glVertex3f(w, 0, d);

	glEnd();
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);
}
void draw_rect_color(int x, int y, int w, int h, float color[3]) {
	glTranslatef(x, y, 0);

	glBegin(GL_QUADS);

	glColor3f(color[0], color[1], color[2]);
	glVertex3f(0, 0, 0);
	
	glVertex3f(0, h, 0);
	
	glVertex3f(w, h, 0);
	
	glVertex3f(w, 0, 0);

	glEnd();
	glLoadIdentity();
}

void clear_screen() {
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
void flip_screen() {
	SDL_GL_SwapBuffers();
}

bool keyisdown(Uint32 Key) {
	Uint8* KeyStates = SDL_GetKeyState(NULL);

	if(KeyStates[Key]) {
		return true;
	}

	return false;
}

// Below was code for the mating function. RIP
// std::vector<std::vector<int> > mix_genome(std::vector<std::vector<int> > g1, std::vector<std::vector<int> > g2, int gene_length) {
// 	std::vector<std::vector<int> > gbuilt = g1; // valgrind gets angry at this a bit...

// 	//syngamete from both parents
// 	for(int i = 0; i < 30; i++) {
// 		int syngamete_chance = floor(rand() % 100);
// 		if(syngamete_chance <= 50) {
// 			gbuilt[i] = g2[i];
// 		}
// 	}

// 	int mutation_chance = floor(rand() % 100);

// 	if(mutation_chance <= 3) {
// 		// cout << "mutated" << endl;

// 		int gene_num = floor(rand() % 30);
// 		int act_num = floor(rand() % gene_length);
// 		int rand_act = floor(rand() % 8);

// 		gbuilt[gene_num][act_num] = rand_act;
// 	}

// 	// cout << "s5: " << gbuilt.size() << endl;

// 	return gbuilt;
// }

std::vector<std::vector<int> > nomate_mutate(std::vector<std::vector<int> > g, int gene_length) {
	std::vector<std::vector<int> > gbuilt = g; // valgrind gets angry at this a bit...

	int mutation_chance = floor(rand() % 100);

	if(mutation_chance <= 4) {
		int gene_num = floor(rand() % 30);
		int act_num = floor(rand() % gene_length);
		int rand_act = floor(rand() % 8);

		gbuilt[gene_num][act_num] = rand_act;
	}

	return gbuilt;
}