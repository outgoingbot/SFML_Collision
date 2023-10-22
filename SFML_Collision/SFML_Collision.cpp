/*
playing with some particles and shit
*/


#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <Windows.h>    
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <RS232Comm.h>
#include <cmath>
//#include "orbit.h"
#include "planet.h"
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define LINE_WIDTH 10
#define MAX_BALLS_COUNT 1000
#define MIN_BALLS_COUNT 1
#define MIN_SIZE_BLACKHOLE 25.f
#define M_PI 3.141592653589793
//Graphics
float myx = 10.0;
float myy = 10.0;
double myexp = 0;
float scaler = 1; //needs to rescale all dots. not just new data
int radioactivity = 200;
//Serial
char charArray[256] = "Empty"; //graphics text buffer
uint32_t numSamples = 0;
float myData;
char incomingData[256] = ""; //Serial Rx Buffer

int dataLength = 255;
int readResult = 0;
float allBallsAvgVelocity = 0;
unsigned int textupdateCounter = 200;
int drawgraph = 0;


sf::Vector2f BHvel;
sf::Vector2f BHpos;
sf::Vector2f BHposTemp;

//i want to pass a struct pointer from main to the orbit class. 
//do i need a types.h that both main and orbit know about?
typedef struct ballparams {
	sf::Vector2f BlackHolePostion = { 10,10 };
	int radioactivity = 2; //maximum launch velocity
	int drawVectorField = 0; //vectorfield lines drawn or not <--(only using this for now)
	float fieldStrength = 0.2f; //strength of the field
} params_t;

params_t BallParams;
sf::RenderWindow* window = nullptr;



bool isMouseOverRect(sf::Vector2f mousePosition, sf::RectangleShape rectangleShape) {
	if (mousePosition.x > rectangleShape.getPosition().x && mousePosition.x < rectangleShape.getPosition().x + rectangleShape.getSize().x) {
		if (mousePosition.y > rectangleShape.getPosition().y && mousePosition.y < rectangleShape.getPosition().y + rectangleShape.getSize().y) {
			return true;
		}
	}
		return false;
}

//adds two vectors
sf::Vector2f operator+ (sf::Vector2f v1, sf::Vector2f v2) {
	return sf::Vector2f(v1.x + v2.x, v1.y + v2.y);
}

//subratcts two vectors
sf::Vector2f operator- (sf::Vector2f v1, sf::Vector2f v2) {
	return sf::Vector2f(v1.x - v2.x, v1.y - v2.y);
}

//returns the dot-product scaler from 2 vectors
float dot(sf::Vector2f v1, sf::Vector2f v2) {
	return (v1.x*v2.x) + (v1.y*v2.y);
}

//magnitude for v2-v1
float mag(sf::Vector2f v1, sf::Vector2f v2) {
	return float(abs(sqrt((v2.x - v1.x)*(v2.x - v1.x) + (v2.y - v1.y)*(v2.y - v1.y))));
}

float sq(float s) {
	return s * s;
}

sf::Vector2f vMultiply(sf::Vector2f v1, sf::Vector2f v2) {
	return sf::Vector2f(v1.x * v2.x, v1.y * v2.y);
}

sf::Vector2f sMultiply(float s, sf::Vector2f v) {
	return sf::Vector2f(s * v.x, s * v.y);
}


int main()
{	

	//sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, 1080), "SFML Playground");
	window = new sf::RenderWindow (sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Playground");
	sf::Vector2f mousePos = (sf::Vector2f) sf::Mouse::getPosition(*window);
	window->setMouseCursorVisible(false);
	window->setFramerateLimit(200);
	sf::Font font;
	if (!font.loadFromFile("../res/Pumpkin_Pancakes.ttf")) {
		printf("Error loading Font");
		system("pause");
	}
	//Make shapes	
	unsigned int numBalls = 1;
	unsigned int numLines = 10;
	std::vector<planet> balls;
	balls.push_back(planet(mousePos, radioactivity,BallParams.fieldStrength ,1)); //create at leat 1 ball and draw the vectorfield pf only ball 1
	
	std::vector<sf::Vertex> lines; //lines that connect all the balls
	lines.push_back(sf::Vertex(sf::Vector2f(0, 0)));


	sf::CircleShape TestBall(30.f);
	TestBall.setOrigin(TestBall.getRadius(), TestBall.getRadius());
	TestBall.setFillColor(sf::Color::Red);
	float PIcounter = 0;
	float orbitRadius = 200;

	sf::CircleShape MouseBlackHole(30.f);
	MouseBlackHole.setOrigin(MouseBlackHole.getRadius(), MouseBlackHole.getRadius());
	MouseBlackHole.setFillColor(sf::Color::Blue);

	sf::Text posText;
	posText.setFont(font);
	posText.setString(charArray); 
	posText.setCharacterSize(30);
	posText.setFillColor(sf::Color::Green);
	posText.setPosition(sf::Vector2f(myx, myy));
	//posText.setOrigin(sf::Vector2f(myx, myy)); //move the shape origin. default 0,0 is top left

	sf::RectangleShape xMouseCross(sf::Vector2f(WINDOW_WIDTH, 2)); //x mouse crosshair
	xMouseCross.setPosition(sf::Vector2f(0, (window->getSize().y / 2) - 1));
	xMouseCross.setFillColor(sf::Color::Magenta);

	sf::RectangleShape yMouseCross(sf::Vector2f(2, WINDOW_HEIGHT)); //y mouse crosshair
	yMouseCross.setPosition(sf::Vector2f((window->getSize().x / 2) - 1, 0));
	yMouseCross.setFillColor(sf::Color::Magenta);
	
	//GUI Objects
	sf::RectangleShape Button_1(sf::Vector2f(100,50)); //create button with wideth,height
	Button_1.setPosition(sf::Vector2f(WINDOW_WIDTH-Button_1.getSize().x, WINDOW_HEIGHT - Button_1.getSize().y));
	Button_1.setFillColor(sf::Color::Blue);

	
	sf::Vertex tsArray[WINDOW_WIDTH * 2];
	for (int y = 0; y < (WINDOW_WIDTH*2)-1; y++) tsArray[y].color = sf::Color::Red;

	sf::Vertex lineInterpol[WINDOW_WIDTH - 1];
	//for (int y = 0; y < WINDOW_WIDTH-1; y++) lineInterpol[y].color = sf::Color::Black;
	

	Serial* SP = new Serial("\\\\.\\COM5");    // adjust as needed \\\\.\\COM10
	if (SP->IsConnected()) printf("We're connected");

	while (window->isOpen())
	{
		//mouse wheel input only? not sure how this 'event' works
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window->close();
				SP->~Serial(); //deconstruct
			}
			else {
				if (event.type == sf::Event::MouseWheelMoved) {
					// display number of ticks mouse wheel has moved
					if (event.mouseWheel.delta > 0) {
						if (numBalls < MAX_BALLS_COUNT) {
							balls.push_back(planet(mousePos, radioactivity)); //add a ball
							lines.push_back(sf::Vertex(sf::Vector2f(0, 0)));
							numBalls++;
							//scaler += 0.01;
							
						}
					}
					if (event.mouseWheel.delta < 0) {
						if (numBalls > MIN_BALLS_COUNT) {
							balls.pop_back();
							lines.pop_back();
							numBalls--;
							//scaler -= 0.01;
						}
					}
					//for (int i = 0; i < WINDOW_WIDTH-1; i++) dot[i].setPosition(sf::Vector2f(i, (float) ( ( (dot[i].getPosition().y - (window.getSize().y / 2)) * scaler ) + (window.getSize().y / 2))) );
				}
			}
		}
		//std::cout << "number of Balls:" << numBalls << "\n";


		//UPDATE LOOP			
			//Display is done at the end of the main loop.

			
		//playing with orbit particles here
		TestBall.setPosition(sf::Vector2f(orbitRadius*cos(PIcounter) + mousePos.x, mousePos.y));
		TestBall.setRadius(30*(cos(PIcounter + (M_PI / 2))+1));
		PIcounter += 0.01f;
		if (PIcounter > 2 * M_PI) PIcounter = 0;
		//


		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			radioactivity++;
			for (int j = 0; j < numBalls; j++) balls[j].maxVel = radioactivity;
			MouseBlackHole.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			radioactivity--;
			for (int j = 0; j < numBalls; j++) balls[j].maxVel = radioactivity;
			MouseBlackHole.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
		}

		mousePos = (sf::Vector2f) sf::Mouse::getPosition(*window);
		for (int j = 0; j < numBalls; j++) balls[j].update(mousePos);
		

		BHposTemp = MouseBlackHole.getPosition();
		MouseBlackHole.setPosition(mousePos);
		BHpos = MouseBlackHole.getPosition();
		BHvel = (BHpos - BHposTemp) / STEP;

	//GUI
		static unsigned int toggle;
		sf::Vector2f mousePosRaw = (sf::Vector2f) sf::Mouse::getPosition(*window);
		if (isMouseOverRect(mousePosRaw, Button_1)) {
			Button_1.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				while (sf::Mouse::isButtonPressed(sf::Mouse::Left));
				toggle++;
				drawgraph = (!(toggle % 2) > 0) ? 1 : 0;
			}
		}

		//Get inputs
		mousePos = (sf::Vector2f) sf::Mouse::getPosition(*window);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Period)) {
			for(int i=0; i<numBalls; i++) balls[i].fieldMod += .1;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Comma)) {
			for (int i = 0; i < numBalls; i++) balls[i].fieldMod -= .1;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			BallParams.fieldStrength += 0.001f;
			MouseBlackHole.setRadius(MIN_SIZE_BLACKHOLE + BallParams.fieldStrength*50);
			MouseBlackHole.setOrigin(MouseBlackHole.getRadius(), MouseBlackHole.getRadius());
			MouseBlackHole.setPosition((sf::Vector2f) sf::Mouse::getPosition(*window));
			for (int j = 0; j < numBalls; j++) balls[j].fieldStrength = BallParams.fieldStrength;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			BallParams.fieldStrength -= 0.001f;
			MouseBlackHole.setRadius(MIN_SIZE_BLACKHOLE + BallParams.fieldStrength*50);
			MouseBlackHole.setOrigin(MouseBlackHole.getRadius(), MouseBlackHole.getRadius());
			MouseBlackHole.setPosition(mousePos);
			for (int j = 0; j < numBalls; j++) balls[j].fieldStrength = BallParams.fieldStrength;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) posText.move(10,0);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) posText.move(-10, 0);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) posText.move(0, -10);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) posText.move(0, 10);
		

		//update crosshair position
		xMouseCross.setPosition(sf::Vector2f(0, mousePos.y));
		yMouseCross.setPosition(sf::Vector2f(mousePos.x, 0));

		//update text
		sf::Vector2f position = posText.getPosition();
		//std::cout << position.x << " " << position.y << "\n";
//		sprintf_s(charArray, "Postion: %f  %f", position.x, position.y);
		textupdateCounter++;
		if (!(textupdateCounter % 50)) {//slow the update text for the avg velocity
			allBallsAvgVelocity = 0;
			for (int j = 0; j < numBalls; j++) allBallsAvgVelocity += balls[j].AvgVel;
			allBallsAvgVelocity /= numBalls;
		}
		sprintf_s(charArray, "Radioactivity: %i\nBalls Count: %i\nAvg Velocity: %f\nField Strength %f\n fieldMod: %f", balls[0].maxVel, numBalls, allBallsAvgVelocity, BallParams.fieldStrength, balls[0].fieldMod);
		posText.setString(charArray);

		
		//Collision Testing------------------------------------------------------------
		sf::Vector2f BallApos;
		float BallARadius;
		sf::Vector2f BallBpos;
		float scaler = 0;

		sf::Vector2f ballA_temp_vel;
		

		//drawing some lines for fun. daisy chin from ball[0] to ball[n]
		for (int i = 0; i < numBalls; i++) {
			lines[i].position = balls[i].Ball->getPosition();
			lines[i].color = sf::Color::Red;
		}
		

		//will need to define some sort of mass for wach ball. ill make it based on the the size
		if (numBalls > 1) {
			for (int a = 0; a<numBalls - 1; a++) {//loop thru each ball
				for (int b = a+1; b<numBalls; b++) { //then complare againsta all other balls

					if (mag(balls[a].PosF, balls[b].PosF) <= balls[a].Radius + balls[b].Radius) {//if collisionm detected
						//need to add a force-distance fix here. so balls dont get stuck
						if (!balls[a].overFlag) {
							balls[a].overFlag = 1;
							//ballA						
//calculate the scaler
							scaler = (2 * balls[b].Mass * dot(balls[a].VelI - balls[b].VelI, balls[a].PosF - balls[b].PosF)) / ((balls[a].Mass + balls[b].Mass) * sq(mag(balls[b].PosF, balls[a].PosF)));
							//multiply the vector with the scaler
							ballA_temp_vel = balls[a].VelI - sMultiply(scaler, balls[a].PosF - balls[b].PosF);

							//ballB
							scaler = (2 * balls[a].Mass * dot(balls[b].VelI - balls[a].VelI, balls[b].PosF - balls[a].PosF)) / ((balls[a].Mass + balls[b].Mass) * sq(mag(balls[a].PosF, balls[b].PosF)));
							balls[b].VelI = balls[b].VelI - sMultiply(scaler, balls[b].PosF - balls[a].PosF);
							//balls[b].VelI.x *= -1;

							//this is shit. its needed to get the collision response here to work on the planet class.
							balls[a].VelI = ballA_temp_vel;
							balls[a].PosI = balls[a].PosF;
							balls[b].PosI = balls[b].PosF;
							balls[a].Time.x = STEP;
							balls[a].Time.y = STEP;
							balls[b].Time.x = STEP;
							balls[b].Time.y = STEP;
						}
					}
					else {
						balls[a].overFlag = 0;
					}


				}
			}
		}
		
		//mouse collision
		if (numBalls > 0) {
			float BHradius = MouseBlackHole.getRadius();
			float BHmass = BHradius * MASSFACTOR*10;
			for (int a = 0; a < numBalls; a++) {//loop thru each ball

					if (mag(balls[a].PosF, BHpos) <= balls[a].Radius + BHradius) {//if collisionm detected
																				  //need to add a force-distance fix here. so balls dont get stuck
						if (!balls[a].overFlag) {
							balls[a].overFlag = 1;
							//ball and mouse collision response						
							//calculate the scaler
							scaler = (2 * BHmass * dot(balls[a].VelI - BHvel, balls[a].PosF - BHpos)) / ((balls[a].Mass + BHmass) * sq(mag(BHpos, balls[a].PosF)));
							//multiply the vector with the scaler
							ballA_temp_vel = balls[a].VelI - sMultiply(scaler, balls[a].PosF - BHpos);

							//this is shit. its needed to get the collision response here to work on the planet class.
							balls[a].VelI = ballA_temp_vel;
							balls[a].PosI = balls[a].PosF;
							balls[a].Time.x = STEP;
							balls[a].Time.y = STEP;
						}
					}
					else {
						balls[a].overFlag = 0;
					}
			}
		}
		//end collision testing---------------------------------------

		

		//display
		window->clear(sf::Color::Black);
		for(int i=0; i<numBalls-1;i+=1)window->draw(&(lines[i]), 2, sf::Lines);

		if (drawgraph) for (int i = 0; i < NUMBER_OF_VECTORS; i++) window->draw(*(balls[0].rectangleSet[i]));
		//window->draw(xMouseCross);
		//window->draw(yMouseCross);
		window->draw(Button_1);
		window->draw(posText);
		for (int i = 0; i < numBalls; i++) window->draw(*(balls[i].Ball));
		window->draw(MouseBlackHole);
		//window->draw(TestBall); //testing a '3d' orbit
		window->display();
	}//end update loop

	return 0;
}