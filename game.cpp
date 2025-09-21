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

// DRAWING AND PRINTING FUNCTIONS //

void DrawString(SDL_Surface* screen, int x, int y, const char* text,
	SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
}


void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	//top left corner
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	//copy width and height
	dest.w = sprite->w;
	dest.h = sprite->h;
	//draw whole sprite on screen at given position
	SDL_Rect temp = { 0,0,21,21 };
	SDL_BlitSurface(sprite, NULL, screen, &dest);
}


void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
}


void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
}


void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
}


void drawFrame(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
}


void drawAll(Game& game, double worldTime) //draw all the picutures and board
{

	double fps = 0;
	char text[128];
	SDL_FillRect(game.board.screen, NULL, game.board.l_green);


	game.board.drawBoard(game.pic);
	game.board.drawPlayer(game.player, game.pic);

	DrawRectangle(game.board.screen, 50, 4, SCREEN_WIDTH - 100, 40, game.board.d_green, game.board.green);
	sprintf(text, "	DONKEY KONG LVL 1	  TIME = %.1lf s   obligatory, 5A: 6points", worldTime);
	DrawString(game.board.screen, game.board.screen->w / 2 - strlen(text) * 8 / 2, 10, text, game.board.charset);
	sprintf(text, "Esc - quit,      n - new game");
	DrawString(game.board.screen, game.board.screen->w / 2 - strlen(text) * 8 / 2, 26, text, game.board.charset);

	SDL_UpdateTexture(game.board.scrtex, NULL, game.board.screen->pixels, game.board.screen->pitch);
	SDL_RenderCopy(game.board.renderer, game.board.scrtex, NULL, NULL);
	SDL_RenderPresent(game.board.renderer);


}

// MOVEMENT AND CONDITIONAL FUNCTIONS //

bool checkUserInput(Player& player, Game& game, SDL_Event& event, Board& board, double& worldTime) { // check user input from keyboard

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) return true;
			else if (event.key.keysym.sym == SDLK_n) {
				board.setPlayerStartingPosistion(player);
				worldTime = 0;
			}
			if (player.isOnLadder)
			{

				if (event.key.keysym.sym == SDLK_UP) {
					player.speed.y = -1 * defaultSpeed;
				}
				if (event.key.keysym.sym == SDLK_DOWN) {
					player.speed.y = defaultSpeed;
				}
			}
			if (event.key.keysym.sym == SDLK_LEFT)  player.speed.x = -1 * defaultSpeed;
			if (event.key.keysym.sym == SDLK_RIGHT)  player.speed.x = defaultSpeed;
			if (event.key.keysym.sym == SDLK_SPACE) {
				if (player.speed.y == 0) {
					player.ifJump = true;
					player.speed.y = -4 * defaultSpeed;
				}

			}
			break;
		case SDL_KEYUP:
			player.speed.y = 0;
			player.speed.x = 0;
			break;
		case SDL_QUIT:
			return true;
		}
	}

	return false;
}


void calculateWorld(Game& game) {


	if (game.player.ifJump || game.player.ifFall) {
		game.player.speed.y += 0.01 * defaultSpeed * game.deltaTime;
	}

	game.player.position.x += game.player.speed.x * game.deltaTime;
	game.player.position.y += game.player.speed.y * game.deltaTime;

}


void climbLadder(Board& board, Player& player) {

	for (int i = 0; i < LADDERS_N; i++) {
		if (player.speed.y >= 0) {
			if ((board.ladders[i].position.x <= player.position.x + player.w) &&
				(board.ladders[i].position.x + BMP_SIZE >= player.position.x) &&
				(board.ladders[i].position.y - 6 <= player.position.y + player.h) &&
				(board.ladders[i].position.y + BMP_SIZE >= player.position.y + player.h))
			{
				player.isOnLadder = true;
				player.ladder_h = board.ladders[i].position.y;
				return;
			}
		}

		if ((board.ladders[i].position.x <= player.position.x + player.w) &&
			(board.ladders[i].position.x + BMP_SIZE >= player.position.x) &&
			(board.ladders[i].position.y - 5 <= player.position.y + player.h) && // for leaving platform
			(board.ladders[i].position.y + BMP_SIZE >= player.position.y + player.h - 2)) { // special check for only bottom of player

			player.isOnLadder = true;
			return;
		}


	}
	player.isOnLadder = false;
}


bool isCollisionWithWalls(Game& game) {
	const double addDistance = 3;
	int walls = 3;

	for (int i = 0; i < walls; i++) {
		// Check if player's right edge collapses with obstacle's left edge
		if ((game.player.position.x + game.player.w >= game.obstacles[i].x - addDistance) &&
			(game.player.position.x + game.player.w <= game.obstacles[i].x + addDistance) &&
			((game.player.position.y + game.player.h > game.obstacles[i].y) &&
				(game.player.position.y < game.obstacles[i].y + game.obstacles[i].h)))
		{
			return true;
		}

		// Check if player's left edge collapses with obstacle's right edge
		if ((game.player.position.x <= game.obstacles[i].x + game.obstacles[i].w + addDistance) &&
			(game.player.position.x >= game.obstacles[i].x + game.obstacles[i].w - addDistance) &&
			((game.player.position.y + game.player.h > game.obstacles[i].y) &&
				(game.player.position.y < game.obstacles[i].y + game.obstacles[i].h)))
		{
			return true;
		}
	}

	return false;
}


bool isCollisionWithLowerEdge(Game& game) {
	const double addDistance = 1;
	for (int i = 0; i < OBSTACLES_N; i++) {
		if ((game.obstacles[i].x - addDistance <= game.player.position.x + game.player.w) &&
			(game.obstacles[i].x + game.obstacles[i].w + addDistance >= game.player.position.x) &&
			(game.obstacles[i].y - addDistance <= game.player.position.y + game.player.h) &&
			(game.obstacles[i].y + game.obstacles[i].h + addDistance >= game.player.position.y))
		{
			if (game.player.position.y + game.player.h <= game.obstacles[i].y) {
				return true;
			}
		}
	}
	return false;
}


bool generalCollision(Game& game) {
	for (int i = 0; i < OBSTACLES_N; i++) {
		if ((game.player.position.x + game.player.w >= game.obstacles[i].x) &&
			(game.player.position.x <= game.obstacles[i].x + game.obstacles[i].w) &&
			((game.player.position.y + game.player.h >= game.obstacles[i].y) &&
				(game.player.position.y <= game.obstacles[i].y + game.obstacles[i].h)))
		{
			return true;
		}
	}
	return false;
}


void collision(Game& game) { // check if collision with any block

	if (isCollisionWithWalls(game))
	{
		game.player.position.x -= 2 * (game.player.speed.x * game.deltaTime);
		game.player.position.y -= 2 * (game.player.speed.y * game.deltaTime);

		game.player.speed.x = 0;
		game.player.speed.y = 0;
		return;
	}
	if (generalCollision(game)) {
		game.player.ifJump = false;
		game.player.ifFall = false;

		if (game.player.isOnLadder) {
			if (isCollisionWithLowerEdge(game)) { // check for if tauching bottom edge of the platform
				if ((game.player.position.y > game.player.ladder_h) && (game.player.speed.y > 0)) { // if is going down
					game.player.position.y -= 2 * (game.player.speed.y * game.deltaTime);
					game.player.speed.y = 0;
				}
			}
			return;
		}


		game.player.position.y -= (game.player.speed.y * game.deltaTime);
		game.player.speed.y = 0;
		return;
	}

	// if player is not colliding with blocks but on ladder
	if (game.player.isOnLadder) {
		game.player.ifFall = false;
		return;
	}

	game.player.ifFall = true;
}


void gameLoop() { // main game loop

	Game game;
	Board board;
	game.startGame();

	double delta, worldTime;
	double t1 = 0, t2 = 0;
	t1 = SDL_GetTicks();
	worldTime = 0;

	while (true) {
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		worldTime += delta;
		t1 = t2;

		game.worldTime = worldTime;
		game.deltaTime = delta * 1000;


		drawAll(game, worldTime);
		bool shouldQuit = checkUserInput(game.player, game, game.event, board, worldTime);
		if (shouldQuit == true) {

			SDL_Quit();
			return;
		}
		calculateWorld(game);
		climbLadder(game.board, game.player);
		collision(game);


	}

}