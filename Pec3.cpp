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
enum GameEntity { Empty, PLAYER, MONSTER, WALL, EXIT };
enum  Direction { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT };
struct Position
{
	int x, y;
	Direction dir;
}  playerPosition, monsterPosition, exitPosition;

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


GameEntity map[SIZE][SIZE] = { // 10x10 matrix
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


int GetSoundAngleFromPositions(Position p1, Position p2)
{
	/*
	 0 = directly in front.
	 90 = directly to the right.
	 180 = directly behind.
	 270 = directly to the left.
	 */
	int deltaX = p2.x - p1.x;
	int deltaY = p2.y - p1.y;
	auto angleInDegrees = 90 + atan2(deltaY, deltaX) * 180 / M_PI;
	int fixedAngle =  static_cast<int>(abs(angleInDegrees)) % 360;
	switch (playerPosition.dir)
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
int GetSoundDistanceFromPositions(Position p1, Position p2, int angle)
{
	int x = abs(p1.x - p2.x);
	int y = abs(p1.y - p2.y);
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
	MonsterAngle = GetSoundAngleFromPositions(playerPosition, monsterPosition);
	MonsterDistance = GetSoundDistanceFromPositions(playerPosition, monsterPosition, MonsterAngle);
	WaterfallAngle = GetSoundAngleFromPositions(playerPosition, exitPosition);
	WaterfallDistance = GetSoundDistanceFromPositions(playerPosition, exitPosition, WaterfallAngle);
	monster.SetPosition(MonsterAngle, MonsterDistance);
	waterfall.SetPosition(WaterfallAngle, WaterfallDistance);
}
void PrintMap()
{
	cout << "*****************************" << endl << endl << endl;

	for (int y = 0; y < SIZE; y++)
	{
		for (int x = 0; x < SIZE; x++)
		{
			std::cout << map[y][x] << " ";
		}
		std::cout << endl;
	}

	cout << endl << endl;
	std::cout << "[Moster] SDL angle " << MonsterAngle << " SDL distance " << MonsterDistance << endl;
	std::cout << "[Waterfall] SDL angle " << WaterfallAngle << " SDL distance " << WaterfallDistance << endl;
	string direction;
	switch (playerPosition.dir)
	{
	case DIR_UP:	direction = "UP"; break;
	case DIR_RIGHT:	direction = "RIGHT"; break;
	case DIR_DOWN:	direction = "DOWN"; break;
	case DIR_LEFT:	direction = "LEFT"; break;
	}
	std::cout << "[Player] direction " << direction << endl;
}
GameEntity Find(Position* pos)
{
	if(pos->x < 0 || pos->x >= SIZE || pos->y < 0 || pos->y >= SIZE)
	{
		return WALL;
	}
	return map[pos->y][pos->x];
}
void TurnPlayer(bool turnLeft)
{
	if (turnLeft)
	{
		switch (playerPosition.dir)
		{
			case DIR_UP:	playerPosition.dir = DIR_LEFT; break;
			case DIR_RIGHT:	playerPosition.dir = DIR_UP; break;
			case DIR_DOWN:	playerPosition.dir = DIR_RIGHT; break;
			case DIR_LEFT:	playerPosition.dir = DIR_DOWN; break;
		}
	}
	else 
	{
		switch (playerPosition.dir)
		{
			case DIR_UP:	playerPosition.dir = DIR_RIGHT; break;
			case DIR_LEFT:	playerPosition.dir = DIR_UP; break;
			case DIR_DOWN:	playerPosition.dir = DIR_LEFT; break;
			case DIR_RIGHT:	playerPosition.dir = DIR_DOWN; break;
		}
	}

	step.Play();
	updatePositions();
	PrintMap();

}
Position* GetPlayerInput()
{
	Position* position = NULL;
	SDL_Event test_event;
	SDL_Scancode key;
	while (SDL_PollEvent(&test_event)) {
		switch (test_event.type) {
		case SDL_KEYDOWN:
			key = test_event.key.keysym.scancode;
			if (key == SDL_SCANCODE_ESCAPE) {
				ExitGame = true;
			}
			else if ((key == SDL_SCANCODE_UP) || (key == SDL_SCANCODE_W))
			{
				position = new Position();
				position->x = playerPosition.x;
				position->y = playerPosition.y;
				position->dir = playerPosition.dir;
				int x, y;
				switch(playerPosition.dir)
				{
					case DIR_UP:
						x = 0;
						y = -1;
						break;
					case DIR_DOWN:
						x = 0;
						y = 1;
						break;
					case DIR_LEFT:
						x = -1;
						y = 0;
						break;
					case DIR_RIGHT:
						x = 1;
						y = 0;
						break;
				}
				position->x += x;
				position->y += y;

			}			
			else if ((key == SDL_SCANCODE_LEFT) || (key == SDL_SCANCODE_A))
			{
				TurnPlayer(true);
			}
			else if ((key == SDL_SCANCODE_RIGHT) || (key == SDL_SCANCODE_D))
			{
				TurnPlayer(false);
			}
			break;

		case SDL_QUIT:
			ExitGame = true;
			break;
		}
	}
	return position;
}
void UpdatePlayerPosition(Position* newPos)
{
	map[playerPosition.y][playerPosition.x] = Empty;
	playerPosition = *newPos;
	map[playerPosition.y][playerPosition.x] = PLAYER;
	updatePositions();
}
void UpdateMonsterPosition(Position* newPos)
{
	map[monsterPosition.y][monsterPosition.x] = Empty;
	monsterPosition = *newPos;
	map[monsterPosition.y][monsterPosition.x] = MONSTER;
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
		int x, y;
		Direction dir;
		int movement = rand() % 7;
		switch (movement)
		{
			case 0:
				x = -1;
				y = -1;
				dir = DIR_LEFT;
			break;
			case 1:
				x = 0;
				y = -1;
				dir = DIR_UP;
			break;
			case 2:
				x = 1;
				y = -1;
				dir = DIR_RIGHT;
			break;
			case 3:
				x = -1;
				y = 0;
				dir = DIR_LEFT;
			break;
			case 4:
				x = 1;
				y = 0;
				dir = DIR_RIGHT;
			break;
			case 5:
				x = -1;
				y = 1;
				dir = DIR_LEFT;
			break;
			case 6:
				x = 0;
				y = 1;
				dir = DIR_DOWN;
			break;
			case 7:
				x = 1;
				y = 1;
				dir = DIR_RIGHT;
			break;
		}
		Position newPosition = monsterPosition;
		newPosition.x += x;
		newPosition.y += y;
		newPosition.dir = dir;
		auto newPlace = Find(&newPosition);
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

	playerPosition.x = 1;
	playerPosition.y = 8;
	playerPosition.dir = DIR_UP;
	map[playerPosition.y][playerPosition.x] = PLAYER;
	monsterPosition.x = 4;
	monsterPosition.y = 3;
	monsterPosition.dir = DIR_DOWN;
	map[monsterPosition.y][monsterPosition.x] = MONSTER;
	exitPosition.x = 9;
	exitPosition.y = 9;
	exitPosition.dir = DIR_DOWN;
	map[exitPosition.y][exitPosition.x] = EXIT;
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
		Position* newPosition = GetPlayerInput();

		if (newPosition != NULL)
		{			
			GameEntity newPlace = Find(newPosition);
			switch (newPlace)
			{
				case Empty:
					UpdatePlayerPosition(newPosition);
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
			delete newPosition;
		}
	}

	cout << "*****************************" << endl;

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