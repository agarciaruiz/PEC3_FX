#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <time.h>
#include "SoundEffect.h"
#include <iostream>
#include <vector>

using namespace std;
#define SIZE 10
enum Grid { Empty, PLAYER, MONSTER, WALL, EXIT };
enum  Direction { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT };
struct Position {
	int x, y;
};
struct Object
{
	Position pos;
	Direction dir;
}  playerObj, monsterObj, exitObj;

int MonsterAngle = 0;
int MonsterDistance = 0;
int WaterfallAngle = 0;
int WaterfallDistance = 0;
bool ExitGame = false;
bool GameResult = false;
double MaxSoundDistance;

SoundEffect death;
SoundEffect gameOver;
SoundEffect wall;
SoundEffect step;
SoundEffect victory;
SoundEffect monster;
SoundEffect monsterStep;
SoundEffect waterfall;


Grid map[SIZE][SIZE] = { // 10x10 matrix
	{ Empty, Empty, Empty, WALL , Empty, Empty, Empty, Empty, Empty, Empty },
	{ Empty, Empty, Empty, WALL , Empty, Empty, Empty, Empty, Empty, Empty },
	{ Empty, WALL , Empty, WALL , Empty, Empty, Empty, Empty, Empty, Empty },
	{ Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, WALL , Empty },
	{ Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, WALL , Empty },
	{ Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, WALL , Empty },
	{ Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, WALL , Empty },
	{ Empty, Empty, WALL , Empty, Empty, Empty, Empty, Empty, WALL , Empty },
	{ Empty, Empty, WALL , Empty, Empty, Empty, Empty, Empty, WALL , Empty },
	{ Empty, Empty, WALL , Empty, Empty, Empty, Empty, Empty, WALL , Empty },
};


int GetSoundAngleFromPositions(Object o1, Object o2)
{
	/*
	 0 = directly in front.
	 90 = directly to the right.
	 180 = directly behind.
	 270 = directly to the left.
	 */
	int deltaX = o2.pos.x - o1.pos.x;
	int deltaY = o2.pos.y - o1.pos.y;
	auto angleInDegrees = 90 + atan2(deltaY, deltaX) * 180 / M_PI;
	int fixedAngle =  static_cast<int>(abs(angleInDegrees)) % 360;
	switch (playerObj.dir)
	{
		case DIR_RIGHT:	fixedAngle -= 90; break;
		case DIR_DOWN:	fixedAngle -= 180; break;
		case DIR_LEFT:	fixedAngle += 90; break;
	}
	if(fixedAngle > 360)
	{
		fixedAngle = static_cast<int>(fixedAngle) % 360;
	}
	if (fixedAngle < 0)
	{
		fixedAngle = 360 + fixedAngle;
	}
	return static_cast<int>(fixedAngle);
}
int GetSoundDistanceFromPositions(Object o1, Object o2, int angle)
{
	int x = abs(o1.pos.x - o2.pos.x);
	int y = abs(o1.pos.y - o2.pos.y);
	double distance = abs(sqrt(pow(x, 2) + pow(y, 2)));
	auto soundDistance = (((distance) * 255) / MaxSoundDistance);

	if(angle > 150 && angle < 210)
	{
		//The sound is behind the player to feel the difference from behind and front with a simple sound system without 5.1 we reduce the sound
		soundDistance += 75;
	}

	return static_cast<int>(soundDistance);
}
void updatePositions()
{
	MonsterAngle = GetSoundAngleFromPositions(playerObj, monsterObj);
	MonsterDistance = GetSoundDistanceFromPositions(playerObj, monsterObj, MonsterAngle);
	WaterfallAngle = GetSoundAngleFromPositions(playerObj, exitObj);
	WaterfallDistance = GetSoundDistanceFromPositions(playerObj, exitObj, WaterfallAngle);
	monster.SetPosition(MonsterAngle, MonsterDistance);
	waterfall.SetPosition(WaterfallAngle, WaterfallDistance);
}
void PrintMap()
{
	for (int y = 0; y < SIZE; y++)
	{
		for (int x = 0; x < SIZE; x++)
		{
			std::cout << map[y][x] << " ";
		}
		std::cout << endl;
	}
	cout << endl << endl;
}
Grid GetGridBox(Object* obj)
{
	if(obj->pos.x < 0 || obj->pos.x >= SIZE || obj->pos.y < 0 || obj->pos.y >= SIZE)
	{
		return WALL;
	}
	return map[obj->pos.y][obj->pos.x];
}
Object* GetPlayerInput()
{
	Object* obj = NULL;
	SDL_Event event;
	SDL_Scancode key;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				key = event.key.keysym.scancode;
				obj = new Object();
				obj->pos.x = playerObj.pos.x;
				obj->pos.y = playerObj.pos.y;
				obj->dir = playerObj.dir;

				if (key == SDL_SCANCODE_ESCAPE) {
					ExitGame = true;
				}
				
				if (key == SDL_SCANCODE_UP)
				{
					obj->pos.y -= 1;
				}			
				else if (key == SDL_SCANCODE_LEFT)
				{
					obj->pos.x -= 1;
				}
				else if (key == SDL_SCANCODE_RIGHT)
				{
					obj->pos.x += 1;
				}
				else if (key == SDL_SCANCODE_DOWN) {
					obj->pos.y += 1;
				}
				break;

			case SDL_QUIT:
				ExitGame = true;
				break;
			}
	}
	return obj;
}
void UpdatePlayerPosition(Object* newObj)
{
	map[playerObj.pos.y][playerObj.pos.x] = Empty;
	playerObj = *newObj;
	map[playerObj.pos.y][playerObj.pos.x] = PLAYER;
	updatePositions();
}
void UpdateMonsterPosition(Object* newObj)
{
	map[monsterObj.pos.y][monsterObj.pos.x] = Empty;
	monsterObj = *newObj;
	map[monsterObj.pos.y][monsterObj.pos.x] = MONSTER;
	updatePositions();
}
void SetGameOver()
{
	monster.Stop();
	waterfall.Stop();
	death.PlayAndWait();
	gameOver.Play();
	ExitGame = true;
}
void ChangeMonsterPosition()
{
	bool validTile = false;
	do
	{
		Object newPosition = monsterObj;
		int movement = rand() % 7;
		switch (movement)
		{
			case 0:
				newPosition.pos.x -= 1;
				newPosition.pos.y -= 1;
				newPosition.dir = DIR_LEFT;
			break;
			case 1:
				newPosition.pos.y -= 1;
				newPosition.dir = DIR_UP;
			break;
			case 2:
				newPosition.pos.x += 1;
				newPosition.pos.y -= 1;
				newPosition.dir = DIR_RIGHT;
			break;
			case 3:
				newPosition.pos.x -= 1;
				newPosition.dir = DIR_LEFT;
			break;
			case 4:
				newPosition.pos.x += 1;
				newPosition.dir = DIR_RIGHT;
			break;
			case 5:
				newPosition.pos.x -= 1;
				newPosition.pos.y += 1;
				newPosition.dir = DIR_LEFT;
			break;
			case 6:
				newPosition.pos.y += 1;
				newPosition.dir = DIR_DOWN;
			break;
			case 7:
				newPosition.pos.x += 1;
				newPosition.pos.y += 1;
				newPosition.dir = DIR_RIGHT;
			break;
		}
		auto newPlace = GetGridBox(&newPosition);
		if(newPlace == Empty)
		{
			UpdateMonsterPosition(&newPosition);
			validTile = true;
		}else if(newPlace == PLAYER)
		{
			validTile = true;
			SetGameOver();
		}
	} while (!validTile);
}
void InitPositions()
{
	MaxSoundDistance = abs(sqrt(pow(SIZE - 1, 2) + pow(SIZE - 1, 2)));

	// Init player object
	playerObj.pos.x = 1;
	playerObj.pos.y = 8;
	playerObj.dir = DIR_UP;
	map[playerObj.pos.y][playerObj.pos.x] = PLAYER;

	// Init monster object
	monsterObj.pos.x = 4;
	monsterObj.pos.y = 3;
	monsterObj.dir = DIR_DOWN;
	map[monsterObj.pos.y][monsterObj.pos.x] = MONSTER;

	// Init exit object
	exitObj.pos.x = 9;
	exitObj.pos.y = 9;
	exitObj.dir = DIR_DOWN;
	map[exitObj.pos.y][exitObj.pos.x] = EXIT;

	updatePositions();
}
void LoadSounds()
{
	death.Init("death.wav");
	gameOver.Init("gameOver.wav");
	wall.Init("shock-wall.wav");
	wall.SetVolume(32);
	step.Init("step-human.wav");
	step.SetVolume(32);
	victory.Init("victory.wav");
	monster.Init("Monster-Snoring.wav");
	monsterStep.Init("step-monster.wav");
	waterfall.Init("waterfall.wav");
}

int main(int argc, char* args[])
{
	SDL_Window* gWindow = NULL;
	
	InitPositions();
	PrintMap();

	srand(static_cast<unsigned int>(time(nullptr)));
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	//Create window
	gWindow = SDL_CreateWindow("PEC3 - Alberto Garcia", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 256, SDL_WINDOW_SHOWN);

	SDL_SetHint("SDL_HINT_RENDER_VSYNC", "1");
	//Sound audio active
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

	LoadSounds();

	monster.PlayLoop();
	waterfall.PlayLoop();
	monster.SetPosition(MonsterAngle, MonsterDistance);
	waterfall.SetPosition(WaterfallAngle, WaterfallDistance);

	while (!ExitGame)
	{
		Object* newObj = GetPlayerInput();

		if (newObj != NULL)
		{			
			Grid newPlace = GetGridBox(newObj);
			switch (newPlace)
			{
				case Empty:
					UpdatePlayerPosition(newObj);
					step.Play();
					monster.SetPosition(MonsterAngle, MonsterDistance);
					waterfall.SetPosition(WaterfallAngle,WaterfallDistance);
					break;
				case WALL:
					wall.Play();
					ChangeMonsterPosition();
					monsterStep.PlayAtPosition(MonsterAngle, MonsterDistance);
					monster.SetPosition(MonsterAngle, MonsterDistance);
					break;
				case MONSTER:
					SetGameOver();
					break;
				case EXIT:
					monster.Stop();
					waterfall.Stop();
					victory.PlayAndWait();
					GameResult = true;
					ExitGame = true;
					break;
			}
			PrintMap();
			delete newObj;
		}
	}

	if(GameResult)
	{
		cout << "Ves la luz del sol, has conseguido escapar de una pieza!" << endl;
	}
	else
	{
		cout << "No es un buen destino el tuyo, eres comida para monstruo!" << endl;
	}
	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	Mix_CloseAudio();
	SDL_Quit();

	return 0;
}