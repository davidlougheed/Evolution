class Food {
	// private:
	// 	int lifetime;
	// 	int maxLifetime;
	// 	int food;
	// 	int water;

	public:
		int xpos;
		int ypos;
		bool dead;
		bool firstflag;

		Food();
		void Kill();
};

Food::Food() {
	dead = false;

	xpos = 0 + floor(rand() % 200);
	ypos = 0 + floor(rand() % 200);

	firstflag = true;
}
void Food::Kill() {
	dead = true;
}