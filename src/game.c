#include "game.h"

void InitGame(GameData *data)
{
	for (int index = 0; index < MAX_POINTS; index ++)
	{
		float centerX = SCREEN_WIDTH / 2;
		float centerY = SCREEN_HEIGHT / 2;
		float radius = 40.0f;


		float x = centerX + (radius * (cos(2 * index * PI / MAX_POINTS)));
		float y = centerY + (radius * (sin(2 * index * PI / MAX_POINTS)));

		Point point = {
			{0,0},	   	// ACCELERATION
			{0,0},	   	// VELOCITY
			{x,y},	   	// POSITION

			20, 		// Radius
			GREEN		// Color
		};

		data->points[index] = point;
	}


	data->springConstant = 0.1;
	for (int index = 0; index < MAX_POINTS; index++)
	{
		for (int innerIndex = index + 1; innerIndex < MAX_POINTS; innerIndex++)
		{
			Point *a = &data->points[index];
			Point *b = &data->points[innerIndex];

			float xDistance = {b->m_position.x - a->m_position.x};
			float yDistance = {b->m_position.y - a->m_position.y};
			float magnitudeOfDistance = sqrt((xDistance * xDistance) + (yDistance * yDistance));
			Spring spring =  { data->springConstant, magnitudeOfDistance, a, b} ;


			switch (index)
			{
				case 0:
					data->springs[innerIndex - 1] = spring;
					break;
				case 1:
					data->springs[innerIndex + 6] = spring;
					break;
				case 2:
					data->springs[innerIndex + 12] = spring;
					break;
				case 3:
					data->springs[innerIndex + 17] = spring;
					break;
				case 4:
					data->springs[innerIndex + 21] = spring;
					break;
				case 5:
					data->springs[innerIndex + 24] = spring;
					break;
				case 6:
					data->springs[innerIndex + 26] = spring;
					break;
				case 7:
					data->springs[innerIndex + 27] = spring;
					break;
			}
		}	
   	}

	
	data->gravity.x = 0;
	data->gravity.y = 0.2;

	data->speed = 1;
	data->jumpAmount = 10;

	data->spawnTimer = 0.0f;
	data->spawnDelay = 0.1f;
}

void UpdateGame(GameData *data, float deltaTime)
{
	(void)deltaTime; // Unused parameter


	updateSprings(data, deltaTime);
	updatePoints(data, deltaTime);
}

void updateSprings(GameData *data, float deltaTime) 
{
	(void) deltaTime;

	for (int index = 0; index < MAX_SPRINGS; index++)
	{
		float xForce = data->springs[index].b->m_position.x - data->springs[index].a->m_position.x;
		float yForce = data->springs[index].b->m_position.y - data->springs[index].a->m_position.y;

		float magnitudeOfForce = sqrt((xForce * xForce) + (yForce * yForce));
    	float displacement = magnitudeOfForce - data->springs[index].restLength;


		Vector2 normalizedForce;
		if (magnitudeOfForce == 0)
		{
			normalizedForce.x = 0;
			normalizedForce.y = 0;
		}
		else
		{
			normalizedForce.x = xForce / magnitudeOfForce;
			normalizedForce.y = yForce / magnitudeOfForce;
		}


		//if (xForce == 0 || yForce == 0)
		//{
		//	normalizedForce.x = 0;
		//	normalizedForce.y = 0;
		//}
		//else
		//{
		//	normalizedForce.x = xForce / magnitudeOfForce;
		//	normalizedForce.y = yForce / magnitudeOfForce;
		//}
		
		normalizedForce.x *= data->springs[index].springConstant * displacement;
		normalizedForce.y *= data->springs[index].springConstant * displacement;

    	data->springs[index].a->m_acceleration.x += normalizedForce.x;
		data->springs[index].a->m_acceleration.y += normalizedForce.y;


		normalizedForce.x *= -1;
		normalizedForce.y *= -1;
    	
		data->springs[index].b->m_acceleration.x += normalizedForce.x;
		data->springs[index].b->m_acceleration.y += normalizedForce.y;
	}
	

}

void updatePoints(GameData *data, float deltaTime)
{
	float damp;

	if (data->spawnTimer > data->spawnDelay)
	{
		damp = 0.97;	
	}
	else
	{
		damp = 0.1;	
		data->spawnTimer+= deltaTime;
	}

	for (int index = 0; index < MAX_POINTS; index++)
	{

		if (IsKeyDown(KEY_LEFT))
		{
			data->points[index].m_velocity.x -= data->speed;
		}	
		
		if (IsKeyDown(KEY_RIGHT))
		{
			data->points[index].m_velocity.x += data->speed;
		}

		if (IsKeyPressed(KEY_SPACE))
		{
			data->points[index].m_velocity.y -= data->jumpAmount;
		}

		data->points[index].m_acceleration.x += data->gravity.x;
		data->points[index].m_acceleration.y += data->gravity.y;

		data->points[index].m_velocity.x *= damp;
		data->points[index].m_velocity.y *= damp;
	
		data->points[index].m_velocity.x += data->points[index].m_acceleration.x;
		data->points[index].m_velocity.y += data->points[index].m_acceleration.y;

		data->points[index].m_position.x += data->points[index].m_velocity.x;
		data->points[index].m_position.y += data->points[index].m_velocity.y;

		ClampPlayerOnScreen(data, deltaTime, index);

    	data->points[index].m_acceleration.x = 0;
		data->points[index].m_acceleration.y = 0;


		
		printf("index: %d ", index);
		printf("x: %f ", data->points[index].m_position.x);
		printf("y: %f\n", data->points[index].m_position.y);
	}

}


void ClampPlayerOnScreen(GameData *data, float deltatime, int index)
{
	(void) deltatime;
		if (data->points[index].m_position.x < data->points[index].m_radius)
		{
			data->points[index].m_position.x = data->points[index].m_radius;
			//data->points[index].m_velocity.x = -1*data->points[index].m_velocity.x;
		}
			
			
		if (data->points[index].m_position.x > SCREEN_WIDTH - data->points[index].m_radius)
		{
			data->points[index].m_position.x = SCREEN_WIDTH - data->points[index].m_radius;
			//data->points[index].m_velocity.x = -1*data->points[index].m_velocity.x;
		}
			

		if (data->points[index].m_position.y < data->points[index].m_radius)
		{
			data->points[index].m_position.y = data->points[index].m_radius;
			//data->points[index].m_velocity.y = -1*data->points[index].m_velocity.y;
		}
			

		if (data->points[index].m_position.y > SCREEN_HEIGHT - data->points[index].m_radius)
		{
			data->points[index].m_position.y = SCREEN_HEIGHT - data->points[index].m_radius;
			//data->points[index].m_velocity.y = -1*data->points[index].m_velocity.y;
		}
}
			




void DrawGame(const GameData *data)
{
	for (int index = 0; index < MAX_POINTS; index++)
	{
		DrawCircle(data->points[index].m_position.x, data->points[index].m_position.y, data->points[index].m_radius, data->points[index].m_color);
	}

	for (int index = 1; index < MAX_SPRINGS; index++)
	{
		DrawLineEx(data->springs[index].a->m_position, data->springs[index].b->m_position, 20, GREEN);
	}
}

void CloseGame(GameData *data)
{
	// Free up and resources textures etc

	free(data); // Free the allocated memory for GameData

	printf("Game Closed!\n");
}