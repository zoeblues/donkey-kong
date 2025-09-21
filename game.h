#pragma once
#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include "game.h"

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}
#ifdef __cplusplus
extern "C"
#endif

// ALL DEFINES //

#define SCREEN_WIDTH	588
#define SCREEN_HEIGHT	500
#define defaultSpeed  0.2f
#define up 10
#define OBSTACLES_N 12
#define floor_h 4
#define FLOORS_N 117 
#define LADDERS_N 24
#define	BMP_SIZE 21
#define FLOOR (SCREEN_HEIGHT - 80)
#define FLOOR_DIS 90 

//SDL FUNCTIONS //

void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset);
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color);
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
void drawFrame(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor);

// ALL STRUCTURES //

struct Position {
	double x = 0;
	double y = 0;
};


struct Speed {
	double x = 0;
	double y = 0;
};


struct Ladder {
	Position position;
};


struct Floor {
	Position position;
};


struct Player {
	Position position;
	Speed speed;
	int w = BMP_SIZE; //width
	int h = BMP_SIZE; //height
	bool ifJump = false;
	bool ifFall = false;
	bool isOnLadder = false;
	int ladder_h = 0;
	SDL_Texture* leoPicture = NULL;


};


struct Pictures {
	SDL_Surface* ladder;
	SDL_Surface* player;
	SDL_Surface* floor;

	void loadAllPictures() {

		this->ladder = SDL_LoadBMP("./ledder.bmp");
		this->player = SDL_LoadBMP("./leo.bmp");
		SDL_SetColorKey(player, true, 0x000000);
		this->floor = SDL_LoadBMP("./floor.bmp");
	}
};


struct Board {

	Floor floors[FLOORS_N];
	Ladder ladders[LADDERS_N];

	SDL_Surface* screen;
	SDL_Surface* charset;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;


	int height = SCREEN_HEIGHT;
	int width = SCREEN_WIDTH;
	int black;
	int green;
	int l_green;
	int d_green;
	int red;
	int blue;


	void drawFloor(int x, int y, int n) {
		int currentX = 0;
		static int currentIndex = 0;
		for (int i = 0; i < n; i++) {
			if (currentIndex < FLOORS_N) {
				floors[currentIndex].position.x = currentX + x;
				floors[currentIndex].position.y = y - BMP_SIZE;
				currentX += BMP_SIZE;
				currentIndex++;
			}
			else {
				// when currentIndex exceeds the array size
				break;
			}
		}
	}


	void drawLedder(int x, int y) {
		int currentY = 0;
		static int currentIndex = 0;
		for (int i = 0; i < floor_h; i++) {
			if (currentIndex < LADDERS_N) {
				ladders[currentIndex].position.x = x;
				ladders[currentIndex].position.y = y - BMP_SIZE + 6 + currentY;
				currentY += BMP_SIZE;
				currentIndex++;
			}
			else {
				// when currentIndex exceeds the array size
				break;
			}
		}
	}


	void setStartingPositions() {

		drawFloor(2, SCREEN_HEIGHT + 10, 29);
		drawFloor(2, FLOOR, 19);
		drawFloor(520, FLOOR, 4);
		drawFloor(2, FLOOR - FLOOR_DIS, 4);
		drawFloor(200, FLOOR - FLOOR_DIS, 20);
		drawFloor(260, FLOOR - 2 * FLOOR_DIS, 10);
		drawFloor(550, FLOOR - 2 * FLOOR_DIS, 3);
		drawFloor(2, FLOOR - 2 * FLOOR_DIS, 8);
		drawFloor(80, FLOOR - 3 * FLOOR_DIS, 20);

		drawLedder(260, SCREEN_HEIGHT - 2 * FLOOR_DIS + up);
		drawLedder(50, SCREEN_HEIGHT - 2 * FLOOR_DIS + up);
		drawLedder(520, SCREEN_HEIGHT - FLOOR_DIS + up);
		drawLedder(200, SCREEN_HEIGHT - FLOOR_DIS + up);
		drawLedder(300, SCREEN_HEIGHT - 3 * FLOOR_DIS + up);
		drawLedder(400, SCREEN_HEIGHT - 4 * FLOOR_DIS + up);

	}


	void setPlayerStartingPosistion(Player& player) {
		player.position.x = SCREEN_WIDTH - BMP_SIZE;
		player.position.y = SCREEN_HEIGHT - BMP_SIZE - up;

	}


	void uploadInitialData() {
		int rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_SetWindowTitle(window, "Zuzanna Warchol DE 197829");
		screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_ShowCursor(SDL_DISABLE);
		charset = SDL_LoadBMP("./cs8x8.bmp");
		SDL_SetColorKey(charset, true, 0x000000);

		black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
		green = SDL_MapRGB(screen->format, 132, 154, 71);
		l_green = SDL_MapRGB(screen->format, 221, 229, 182);
		d_green = SDL_MapRGB(screen->format, 84, 98, 45);

	}


	void drawBoard(Pictures& pic)
	{
		// draw floors
		for (int i = 0; i < FLOORS_N; i++) {
			DrawSurface(screen, pic.floor, floors[i].position.x, floors[i].position.y);
		}

		//draw ladders
		for (int i = 0; i < LADDERS_N; i++) {
			DrawSurface(screen, pic.ladder, ladders[i].position.x, ladders[i].position.y);
		}
	}


	void drawPlayer(Player& player, Pictures& pic) {
		DrawSurface(screen, pic.player, player.position.x, player.position.y);
	}
};


struct Game {

	Player player;
	SDL_Event event;
	Board board;
	Pictures pic;
	SDL_Rect obstacles[OBSTACLES_N];
	double worldTime = 0;
	double deltaTime = 0;

	void setObstacles() {
		obstacles[0] = { 0,SCREEN_HEIGHT - 10,SCREEN_WIDTH + BMP_SIZE, BMP_SIZE }; // main floor
		obstacles[1] = { SCREEN_WIDTH + 14,0,1,SCREEN_HEIGHT }; //right wall
		obstacles[2] = { 0,0,6,SCREEN_HEIGHT }; // left wall
		obstacles[3] = { 0, FLOOR - BMP_SIZE, 19 * BMP_SIZE, BMP_SIZE };
		obstacles[4] = { 520,FLOOR - BMP_SIZE, 4 * BMP_SIZE,BMP_SIZE };
		obstacles[5] = { 0,FLOOR - BMP_SIZE - FLOOR_DIS, 4 * BMP_SIZE,BMP_SIZE };
		obstacles[6] = { 200,FLOOR - BMP_SIZE - FLOOR_DIS,20 * BMP_SIZE,BMP_SIZE };
		obstacles[7] = { 260,FLOOR - BMP_SIZE - 2 * FLOOR_DIS,10 * BMP_SIZE,BMP_SIZE };
		obstacles[8] = { 550,FLOOR - BMP_SIZE - 2 * FLOOR_DIS,3 * BMP_SIZE,BMP_SIZE };
		obstacles[9] = { 2, FLOOR - BMP_SIZE - 2 * FLOOR_DIS, 8 * BMP_SIZE,BMP_SIZE };
		obstacles[10] = { 80, FLOOR - BMP_SIZE - 3 * FLOOR_DIS,20 * BMP_SIZE,BMP_SIZE };
		obstacles[11] = { 0,0, SCREEN_WIDTH + BMP_SIZE, BMP_SIZE }; //ceiling
	}


	void startGame() {
		board.setStartingPositions();
		board.setPlayerStartingPosistion(player);
		board.uploadInitialData();
		pic.loadAllPictures();
		setObstacles();
	}
};



// DRAWING AND PRINTING FUNCTIONS //

void drawAll(Game& game, double worldTime);

// MOVEMENT AND CONDITIONAL FUNCTIONS //

bool checkUserInput(Player& player, Game& game, SDL_Event& event, Board& board, double& worldTime);
void calculateWorld(Game& game);
void climbLadder(Board& board, Player& player);
bool isCollisionWithWalls(Game& game);
bool isCollisionWithLowerEdge(Game& game);
bool generalCollision(Game& game);
void collision(Game& game);
void gameLoop();
