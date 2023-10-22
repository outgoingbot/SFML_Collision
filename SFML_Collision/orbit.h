#ifndef _orbit
#define _orbit

#include <SFML/Graphics.hpp>
#include <cmath>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define LINE_WIDTH 10
#define BALL_RADIUS 25.f
#define NUMBER_OF_VECTORS 2500
#define NUMBER_OF_VECTORS_PER_ROW 50
#define NUMBER_OF_VECTORS_PER_COLUMN 50

#define STEP 0.02f
#define res 100

//extern int main_maxVel;

	class orbit {
	public:		
		int Xop = 32, Yop = 16; //orbit point
		float timeK = 0, timeX = 0;
		float Xf = 1, Yf = 1; //set Final X , //set Final Y
		float Xc = random(0, WINDOW_WIDTH), Yc = random(2, WINDOW_HEIGHT);  //Current Position based on current time
		float Vxi = random(-30, 30), Vyi = random(-30, 30); //m/s
		float Xf_temp = 1, Yf_temp = 1;
		float Vxf = 0, Vyf = 0;
		float AvgVel = 10;
		unsigned int ZeroVelocityCount, ZeroVelocityLimit;
		//uint8_t r, g, b;
		sf::CircleShape* Ball = nullptr;
		sf::RectangleShape* LineVectors = nullptr;
		std::vector<sf::RectangleShape*> rectangleSet;
		int maxVel = 1;
		float fieldStrength = 0.2; //strength of the field
		float orbitDir = 1;

		//this is where i will use the vxField and vyField to rotate each rectangle
		int drawVectorFiledON = 0;
		float commandedAngle[NUMBER_OF_VECTORS] = { 0 };
		float magnitude2, Opp, Adj, error, currentX[NUMBER_OF_VECTORS], currentY[NUMBER_OF_VECTORS];
		int idx = 0;

		//returns a random number LOW (inclusive) to HIGH (exclusive)
		int random(int low, int high) {
			float temp = (float)(rand() % res);
			temp = temp / res;
			return (int)((temp * (high - low)) + low);
		}

		orbit(sf::Vector2f mousePosBall, int radioactivity = 2, float fieldmag = 0.2f, int drawVectorFiledONOFF = 0);
		//~orbit();
		void update(sf::Vector2f mousePosBall); //update the snake postion and the screen
		void resetBall();
		
	protected:
	
	private:
		
	};



#endif