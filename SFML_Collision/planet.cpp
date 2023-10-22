#include "planet.h"


planet::planet(sf::Vector2f mousePosBall, int radioactivity, float fieldmag, int drawVectorFiledONOFF) { //constructor
		drawVectorFiledON = drawVectorFiledONOFF;
		ZeroVelocityCount = 0;
		ZeroVelocityLimit = 100; //number of screen updates with the ball absoulte value of the average velocity is < 1 before resetting the ball
		
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
		Radius = random(10.0f, 50.0f);
		Mass = Radius * MASSFACTOR;//0.000010f is a good mass?
		Ball = new sf::CircleShape(Radius);
		Ball->setOrigin(Radius, Radius);
		Ball->setPosition(mousePosBall.x-1, mousePosBall.y-1);//hack. Cant start the ball no at same point as mouse
		OrbitPoint = mousePosBall;
		maxVel = radioactivity;
		fieldStrength = fieldmag;
		wallFriction = 0.8f;
		resetPlanet();
	}

	void planet::update(sf::Vector2f mousePosBall) { //Velocity Control of ball
		OrbitPoint = mousePosBall;
		if (ZeroVelocityCount != ZeroVelocityLimit) { //if the avg velocity of the ball has been has been low for some time

			// control the position froma velocity
			PosF = PosI + vMultiply(VelI, Time);

			//calculate the i,j components of the vector field
			float magnitude = getMag(OrbitPoint, PosF);
			sf::Vector2f Vfield;
			//circular
			//Vfield.x = (orbitDir*(PosF.y - OrbitPoint.y) - fieldMod * (PosF.x - OrbitPoint.x)) / magnitude; //circular attractor field
			//Vfield.y = (-orbitDir * (PosF.x - OrbitPoint.x) - fieldMod * (PosF.y - OrbitPoint.y)) / magnitude;	
			
			//vertical downward
			Vfield.x = (fieldMod * (PosF.x - OrbitPoint.x)) / magnitude;;
			Vfield.y =  (fieldMod * (PosF.y - OrbitPoint.y)) / magnitude;
						
			   //i didant implements any of this below (gravity stuff)
			//G = 6.673E-11 (newton's gravitational constant)
			//M = 5.98E24 (mass of Earth)
			//m = mass of satellite
			//R is distance of Satellite from Earth

			//define the position of the Earth from the mouse position
			//set the Mass for the Earth and the Satellite
			//calculate the vector between the Earth and the Satellite
			//calulate the Centripital force magnitude: F1=(G*M*m)/R^2
			//calculate the Centrifugal force magnitude: F2=(mass*velocity^2)/R
			//use the kinematic equation to solve Force to Position !
			//X = Xo + Vo*t + a*t^2
			//where a is little g

				   
	
			//update the velocity with the VectField			
			//VelI += fieldStrength * Vfield;
			//Velocities Calc
			//VelF = (PosF - PosF_temp) / STEP;
			//PosF_temp = PosF;
			//float w1 = 0.1f, w2 = 0.9f;
			//AvgVel = w2*AvgVel + w1*getMag(VelF);//this math is not good

			//if (AvgVel < 1 || isVectInProx(OrbitPoint, PosF,100)) ZeroVelocityCount++; //increment on crash/stuck

			if (PosF.y <= 0 || PosF.y >= WINDOW_HEIGHT - 1) { //hit the ground or ceiling
				VelI.y = PosF.y >= WINDOW_HEIGHT - 1 ? -(abs(wallFriction*VelI.y)) : abs(wallFriction*VelI.y);
				PosI.y = PosF.y >= WINDOW_HEIGHT - 1 ? WINDOW_HEIGHT - 1 : 0;//set the ball origin back to the edge
				Time.y = 0;
			}
			else if (PosF.x <= 0 || PosF.x >= WINDOW_WIDTH - 1) { //hit either wall
				VelI.x = PosF.x >= WINDOW_WIDTH - 1 ? -abs(wallFriction*VelI.x) : abs(wallFriction*VelI.x);
				PosI.x = PosF.x >= WINDOW_WIDTH - 1 ? WINDOW_WIDTH - 1 : 0;
				Time.x = 0;
			}

			Ball->setPosition(PosF);
			Time.x += STEP;
			Time.y += STEP;

			//trying make vector field visual-------------------------------------------------------------------------------------------------			
			if (drawVectorFiledON) {
				for (int x = 0; x < NUMBER_OF_VECTORS_PER_ROW; x++) {
					for (int y = 0; y < NUMBER_OF_VECTORS_PER_COLUMN; y++) {
						idx = (NUMBER_OF_VECTORS_PER_ROW)*y + x;
						//math here to get the angle from the vector field.
						magnitude2 = abs(sqrt((currentX[idx] - OrbitPoint.x)*(currentX[idx] - OrbitPoint.x) + (currentY[idx] - OrbitPoint.y)*(currentY[idx] - OrbitPoint.y)));
						Opp = (orbitDir*(currentY[idx] - OrbitPoint.y) - fieldMod * (currentX[idx] - OrbitPoint.x)) / magnitude2; //circular attractor field
						Adj = (-orbitDir * (currentX[idx] - OrbitPoint.x) - fieldMod * (currentY[idx] - OrbitPoint.y)) / magnitude2;
						commandedAngle[idx] = atan2f(Adj, Opp) * 180 / 3.141592653589793; //not sure if i need to check for NAN from atan2f()
						//rotate here
						error = commandedAngle[idx] - rectangleSet[idx]->getRotation();
						rectangleSet[idx]->rotate(error);
					}
				}
			}
			//---------------------------------------------------------------------------------------------------------------------------------------

		} else {
			//resetPlanet(); //restart the ball
		}
	}

	void planet::resetPlanet() {
		//Serial.printf("Restart %u\r\n", ZeroVelocityCount);
		ZeroVelocityCount = 0;
		Time.x = STEP; Time.y = STEP;
		PosF.x = 1;
		PosF.y = 1;
		PosI.x = random(0, WINDOW_WIDTH);//OrbitPoint.x - 1;
		PosI.y = random(0, WINDOW_HEIGHT);//OrbitPoint.y - 1;
		PosF_temp.x = 1, PosF_temp.y = 1;
		VelF.x = 0, VelF.y = 0;
		AvgVel = 10;
		orbitDir = random(0, 2) > 0 ? 1 : -1;
		Ball->setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
		VelI.x = random(-maxVel, maxVel);
		VelI.y = random(-maxVel, maxVel); //m/s
		//VelI.x = 0;
		//VelI.y = 0; //m/s

		printf("VelI.x: %f\tVelI.y: %f\r\n", VelI.x, VelI.y);

	}

	//check if vector Moving is over vector Base +- a radious of acceptance
	bool planet::isVectInProx(sf::Vector2f vBase, sf::Vector2f vMoving, float radius) {
		if (vMoving.x > vBase.x-radius && vMoving.x < vBase.x + radius) {
			if (vMoving.y > vBase.y-radius && vMoving.y < vBase.y + radius) {
				return true;
			}
		}
		return false;
	}

	//change the name from getMag to get Distance between 2 verticies
	float planet::getMag(sf::Vector2f vBase, sf::Vector2f vMoving) {
		return float (abs(sqrt((vMoving.x - vBase.x)*(vMoving.x - vBase.x) + (vMoving.y - vBase.y)*(vMoving.y - vBase.y))));
	}

	sf::Vector2f planet::vMultiply(sf::Vector2f vBase, sf::Vector2f vMoving) {
		return sf::Vector2f(vMoving.x * vBase.x, vMoving.y * vBase.y);
	}

	float planet::getMag(sf::Vector2f vMoving) {
		return float(abs(sqrt(vMoving.x*vMoving.x + vMoving.y*vMoving.y)));
	}

