#include "Orbit.h"


	orbit::orbit(sf::Vector2f mousePosBall, int radioactivity, float fieldmag, int drawVectorFiledONOFF) { //constructor
		drawVectorFiledON = drawVectorFiledONOFF;
		ZeroVelocityCount = 0;
		ZeroVelocityLimit = 150; //number of screen updates with the ball absoulte value of the average velocity is < 1 before resetting the ball
		
		
		if (drawVectorFiledON) { //we only want one set of vector field rectangle objects to be created
			for (int i = 0; i < NUMBER_OF_VECTORS; i++) {
				LineVectors = new sf::RectangleShape(sf::Vector2f(20, 5));
				rectangleSet.push_back(LineVectors);
				rectangleSet[i]->setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
			}
			for (int x = 0; x < NUMBER_OF_VECTORS_PER_ROW; x++) {
				for (int y = 0; y < NUMBER_OF_VECTORS_PER_COLUMN; y++) {
					int idx = (NUMBER_OF_VECTORS_PER_ROW)*y + x;
					rectangleSet[idx]->setPosition(((WINDOW_WIDTH / NUMBER_OF_VECTORS_PER_ROW)*x + WINDOW_WIDTH / NUMBER_OF_VECTORS_PER_ROW / 2), ((WINDOW_HEIGHT / NUMBER_OF_VECTORS_PER_COLUMN)*y) + WINDOW_HEIGHT / NUMBER_OF_VECTORS_PER_COLUMN / 2);
					currentX[idx] = rectangleSet[idx]->getPosition().x;
					currentY[idx] = rectangleSet[idx]->getPosition().y;
				}
			}
		}
		float ballRadius = random(2.f, 10.f);
		Ball = new sf::CircleShape(ballRadius);
		Ball->setOrigin(ballRadius, ballRadius);
		Ball->setPosition(mousePosBall.x, mousePosBall.y);
		
		Xop = mousePosBall.x;
		Yop = mousePosBall.y;
		maxVel = radioactivity;
		fieldStrength = fieldmag;
		resetBall();
	}

	void orbit::update(sf::Vector2f mousePosBall) { //Velocity Control of ball
		Xop = mousePosBall.x;
		Yop = mousePosBall.y;
		if (!(Xf<-50 || Xf>WINDOW_WIDTH + 50 || ZeroVelocityCount == ZeroVelocityLimit)) {
			float magnitude = abs(sqrt((Xf - Xop)*(Xf - Xop) + (Yf - Yop)*(Yf - Yop)));
			//float VxField = -(Xf-Xop)/magnitude; //attractor field
			//float VyField = -(Yf-Yop)/magnitude;
			float VxField = (orbitDir*(Yf - Yop) - 2.5*(Xf - Xop)) / magnitude; //circular attractor field
			float VyField = (-orbitDir * (Xf - Xop) - 2.5*(Yf - Yop)) / magnitude;
			//float maxVel = 0.2;

			//trying make vector field visual-------------------------------------------------------------------------------------------------			
			if (drawVectorFiledON) {
				for (int x = 0; x < NUMBER_OF_VECTORS_PER_ROW; x++) {
					for (int y = 0; y < NUMBER_OF_VECTORS_PER_COLUMN; y++) {
						idx = (NUMBER_OF_VECTORS_PER_ROW)*y + x;
						//math here to get the angle from the vector field.
						magnitude2 = abs(sqrt((currentX[idx] - Xop)*(currentX[idx] - Xop) + (currentY[idx] - Yop)*(currentY[idx] - Yop)));
						Opp = (orbitDir*(currentY[idx] - Yop) - 2.5*(currentX[idx] - Xop)) / magnitude2; //circular attractor field
						Adj = (-orbitDir * (currentX[idx] - Xop) - 2.5*(currentY[idx] - Yop)) / magnitude2;
						commandedAngle[idx] = atan2f(Adj, Opp) * 180 / 3.141592653589793;; //not sure if i need to check for NAN from atan2f()
						//rotate here
						error = commandedAngle[idx] - rectangleSet[idx]->getRotation();
						rectangleSet[idx]->rotate(error);
					}
				}
			}
			//---------------------------------------------------------------------------------------------------------------------------------------
		
			// control the position froma velocity
			Xf = Xc + Vxi * timeX;
			Yf = Yc + Vyi * timeK;
			//float attractorMagnitude = 0.2;
			//update the velocity			
			Vxi += fieldStrength * VxField;
			Vyi += fieldStrength * VyField;

			//Velocities Calc
			Vxf = (Xf - Xf_temp) / STEP; //update the final velocity (final means all times except time=0)
			Vyf = (Yf - Yf_temp) / STEP;
			Xf_temp = Xf;
			Yf_temp = Yf;
			AvgVel = (AvgVel + (abs(Vxf) + abs(Vyf))) / 2;

			//if (AvgVel < 1 || (int16_t)Xf == (int16_t)Xop || (int16_t)Yf == (int16_t)Yop) ZeroVelocityCount++; //increment on crash/stuck
			if (AvgVel < 1 || (((int)Xf > (int)Xop - 100 && (int)Xf < (int)Xop + 100) && ((int)Yf > (int)Yop - 100 && (int)Yf < (int)Yop + 100))) ZeroVelocityCount++; //increment on crash/stuck

			if (Yf <= 0 || Yf >= WINDOW_HEIGHT - 1) { //hit the ground or ceiling
				Vyi = Yf >= WINDOW_HEIGHT - 1 ? -abs(Vyf) : abs(Vyf);
				Yc = Yf >= WINDOW_HEIGHT - 1 ? WINDOW_HEIGHT - 1 : 0;
				timeK = 0;
			}
			else if (Xf <= 0 || Xf >= WINDOW_WIDTH - 1) { //hit either wall
				Vxi = Xf >= WINDOW_WIDTH - 1 ? -abs(Vxf) : abs(Vxf);
				Xc = Xf >= WINDOW_WIDTH - 1 ? WINDOW_WIDTH - 1 : 0;
				timeX = 0;
			}

			Ball->setPosition(sf::Vector2f(Xf,Yf));
			timeK += STEP;
			timeX += STEP;
		}
		else { //restart the ball
			resetBall();
		}
	}

	void orbit::resetBall() {
		//Serial.printf("Restart %u\r\n", ZeroVelocityCount);
		ZeroVelocityCount = 0;
		timeK = 0;
		timeX = 0;
		Xf = 1, Yf = 1; //set Final X , //set Final Y
		Xc = Xop + 1, Yc = Yop + 1;  //Current Position based on current time
		Vxi = random(-maxVel, maxVel), Vyi = random(-maxVel, maxVel); //m/s
		Xf_temp = 1, Yf_temp = 1;
		Vxf = 0, Vyf = 0;
		AvgVel = 10;
		orbitDir = random(0, 2) > 0 ? 1 : -1;
		Ball->setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
		//SET NEW COLOR HERE
	}
