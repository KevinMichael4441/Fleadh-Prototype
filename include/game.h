#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "math.h"

// GameData holds all the game related data
// such as score, player info, level info etc.
// Its passed to all game functions as a pointer
// in order to read and modify the game state.
// As its a pointer its efficient to pass around
// But we must manage its memory properly to avoid memory leaks. 
// Free GameData pointer on exit. See CloseGame()

typedef struct Point
{
	Vector2 m_acceleration;
	Vector2 m_velocity;
	Vector2 m_position;

	float m_radius;
	Color m_color;
} Point;

typedef struct Spring
{
	float springConstant;
	float restLength;

	Point *a;
	Point *b;

} Spring;



typedef struct GameData
{
	Point points[MAX_POINTS];
	Spring springs[MAX_SPRINGS];

	float springConstant;
	float speed;
	float jumpAmount;
	Vector2 gravity;

	float spawnTimer;
	float spawnDelay;
} GameData;

void InitGame(GameData *data);
void UpdateGame(GameData *data, float deltaTime);

void updatePoints(GameData *data, float deltaTime);
void updateSprings(GameData *data, float deltaTime);

void ClampPlayerOnScreen(GameData *data, float deltatime, int index);
void DrawGame(const GameData *data);
void CloseGame(GameData *data);

#endif // GAME_H