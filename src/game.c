#include "game.h"

void InitGame(GameData *data)
{
	data->relapseTimer = 0.0f;
	data->inputRecieved = false;

	for (int index = 0; index < MAX_BEZIER_POINTS; index ++)
	{
		data->bezierPoints[index] = (Vector2){0,0};
		data->bezierMoving[index] = false;
	}

	for (int index = 0; index < MAX_POINTS; index ++)
	{
		float centerX = SCREEN_WIDTH / 2;
		float centerY = SCREEN_HEIGHT / 2;
		float radius = 24.0f;


		float x = centerX + (radius * (cos(2 * index * PI / MAX_POINTS)));
		float y = centerY + (radius * (sin(2 * index * PI / MAX_POINTS)));

		Point point = {
			{0,0},	   	// ACCELERATION
			{0,0},	   	// VELOCITY
			{x,y},	   	// POSITION

			false,
			1, 			// Radius
			GREEN		// Color
		};

		data->points[index] = point;
	}


	data->springConstant = 0.01;
	for (int currentPoint = 0; currentPoint < MAX_POINTS; currentPoint++)
	{
		for (int nextPoint = currentPoint + 1 ; nextPoint < MAX_POINTS; nextPoint++)
		{
			Point *a = &data->points[currentPoint];
			Point *b = &data->points[nextPoint];

			float xDistance = {b->m_position.x - a->m_position.x};
			float yDistance = {b->m_position.y - a->m_position.y};
			float magnitudeOfDistance = sqrt((xDistance * xDistance) + (yDistance * yDistance));
			Spring spring =  { data->springConstant, magnitudeOfDistance, a, b} ;


			switch (currentPoint)
			{
				case 0:
					data->springs[nextPoint - 1] = spring;
					break;
				case 1:
					data->springs[nextPoint + 6] = spring;
					break;
				case 2:
					data->springs[nextPoint + 12] = spring;
					break;
				case 3:
					data->springs[nextPoint + 17] = spring;
					break;
				case 4:
					data->springs[nextPoint + 21] = spring;
					break;
				case 5:
					data->springs[nextPoint + 24] = spring;
					break;
				case 6:
					data->springs[nextPoint + 26] = spring;
					break;
				case 7:
					data->springs[nextPoint + 27] = spring;
					break;					
			}
		}	
   	}

	
	data->gravity.x = 0;
	data->gravity.y = 0.1;

	data->speed = 2;
	data->jumpAmount = 10;

	data->spawnTimer = 0.0f;
	data->spawnDelay = 0.1f;
}

void resetBlob(GameData* data)
{
	Vector2 center = {0,0};
	float radius = 1;

	for (int index = 0; index < MAX_POINTS; index++)
	{
		float x = center.x + (radius * (cos(2 * index * PI / MAX_POINTS)));
		float y = center.y + (radius * (sin(2 * index * PI / MAX_POINTS)));

		data->points[index].m_acceleration.x += x;
		data->points[index].m_acceleration.y += y;
	}
}

void UpdateGame(GameData *data, float deltaTime)
{
	(void)deltaTime; // Unused parameter

	if (data->inputRecieved)
	{
		if (data->relapseTimer > RELAPSE_DELAY)
		{
			resetBlob(data);
			data->relapseTimer = 0.0f;
			data->inputRecieved = false;
		}
		else
		{
			data->relapseTimer += deltaTime;
		}
	}

	updateSprings(data, deltaTime);
	updatePoints(data, deltaTime);
	updateBezierPoints(data);
}

void updateBezierPoints(GameData* data)
{
	data->bezierPoints[MAX_BEZIER_POINTS - 1] = data->points[0].m_position;

	for (int index = 0; index < MAX_POINTS; index++)
	{
		data->bezierPoints[index * 2] = data->points[index].m_position;
	}


	for (int index = 1; index < MAX_BEZIER_POINTS - 1; index += 2)
	{
		Vector2 controlPoint = calculateControlPoint(data->bezierPoints[index - 1], data->bezierPoints[index + 1]);
		data->bezierPoints[index] = controlPoint;
	}

	for (int index = 0; index < MAX_BEZIER_POINTS; index++)
	{
		Vector2 point = data->bezierPoints[index];
	}

}





Vector2 calculateControlPoint(Vector2 t_m, Vector2 t_n)
{
	float offset = 10;
	float x = (t_m.x + t_n.x) / (float)2;
	float y = (t_m.y + t_n.y) / (float)2;

	

	Vector2 controlPoint = {x - offset * sin(90), y + offset * cos(90)};

	return controlPoint;
}

void updateSprings(GameData *data, float deltaTime) 
{
	(void) deltaTime;

	for (int index = 0; index < MAX_SPRINGS; index++)
	{

		float xForce = data->springs[index].b->m_position.x - data->springs[index].a->m_position.x;
		float yForce = data->springs[index].b->m_position.y - data->springs[index].a->m_position.y;

		float magnitudeOfForce = sqrt((xForce * xForce) + (yForce * yForce));

		float displacement;

		if (data->springs[index].b->m_lock || data->springs[index].b->m_lock)
		{
			displacement = magnitudeOfForce - data->springs[index].restLength * 20;
		}
    	else
		{
			displacement = magnitudeOfForce - data->springs[index].restLength;

		}

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

		if (!data->springs[index].a->m_lock)
		{
    		data->springs[index].a->m_acceleration.x += normalizedForce.x;
			data->springs[index].a->m_acceleration.y += normalizedForce.y;
		}


		normalizedForce.x *= -1;
		normalizedForce.y *= -1;
    	
		if (!data->springs[index].b->m_lock)
		{
			data->springs[index].b->m_acceleration.x += normalizedForce.x;
			data->springs[index].b->m_acceleration.y += normalizedForce.y;
		}
		
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
			data->inputRecieved = true;
		}	
		
		if (IsKeyDown(KEY_RIGHT))
		{
			data->points[index].m_velocity.x += data->speed;
			data->inputRecieved = true;
		}

		if (IsKeyPressed(KEY_SPACE))
		{
			data->points[index].m_velocity.y -= data->jumpAmount;
			data->inputRecieved = true;
		}

		if (IsKeyDown(KEY_Z) && !data->pointsLocked)
		{
			lockPoints(data);
			data->pointsLocked = true;
			data->inputRecieved = true;
		}

		if (IsKeyReleased(KEY_Z))
		{
			unlockPoints(data);
			data->pointsLocked = false;
		}

		if (!data->points[index].m_lock)
		{
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
		}



		
		printf("index: %d ", index);
		printf("x: %f ", data->points[index].m_position.x);
		printf("y: %f\n", data->points[index].m_position.y);
	}

	for (int index = 0; index < MAX_POINTS + 1; index++)
	{
		if (index == MAX_POINTS)
		{
			data->pointCoords[index] = data->points[0].m_position;
		}
		else
		{
			data->pointCoords[index] = data->points[index].m_position;
		}
	}

}

void lockPoints(GameData *data)
{
	if (data->pointsLocked)
	{
		return;
	}

	for (int index = 0; index < MAX_POINTS; index++)
	{
		float pointMinX = data->points[index].m_position.x - data->points[index].m_radius;
		float pointMaxX = data->points[index].m_position.x + data->points[index].m_radius;
		float pointMaxY = data->points[index].m_position.y + data->points[index].m_radius;
		
	    if (pointMinX <= SCREEN_WIDTH &&
    		pointMaxX >= 0 &&
    		pointMaxY >= SCREEN_HEIGHT )
			{
				data->points[index].m_lock = true;
			}
	}
}

void unlockPoints(GameData *data)
{
	for (int index = 0;  index < MAX_POINTS; index++)
	{
		data->points[index].m_lock = false;
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


Vector2 calculateCenter(GameData* data)
{
	float sumX = 0.0f;
	float sumY = 0.0f;

	for (int index = 0; index < MAX_POINTS; index++)
	{
		sumX += data->points[index].m_position.x;
		sumY += data->points[index].m_position.y;
	}

	float x = sumX / MAX_POINTS;
	float y = sumY / MAX_POINTS;
	
	return (Vector2){x,y};
}




void DrawGame(GameData *data)
{
	DrawSplineBezierQuadratic(data->bezierPoints, MAX_BEZIER_POINTS, 5, GREEN);
	//for (int index = 0; index < MAX_POINTS; index++)
	//{
	//	DrawCircle(data->points[index].m_position.x, data->points[index].m_position.y, data->points[index].m_radius, data->points[index].m_color);
	//}

	//for (int index = 1; index < MAX_SPRINGS; index++)
	//{
	//	DrawLineEx(data->springs[index].a->m_position, data->springs[index].b->m_position, 24, GREEN);
	//}

	//for (int index = 0; index < MAX_POINTS; index++)
	//{
	//	if (index == MAX_POINTS - 1)
	//	{
	//		DrawLineBezier(data->points[index].m_position, data->points[0].m_position, 5, GREEN);
	//	}
	//	else
	//	{
	//		DrawLineBezier(data->points[index].m_position, data->points[index+1].m_position, 5, GREEN);
	//	}
	//}
}

void CloseGame(GameData *data)
{
	// Free up and resources textures etc
	UnloadTexture(data->playerTexture);
	free(data); // Free the allocated memory for GameData
	
	printf("Game Closed!\n");
}