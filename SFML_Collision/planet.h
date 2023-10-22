#ifndef _planet
#define _planet

#include <SFML/Graphics.hpp>
#include <cmath>
//#include <time.h>
#include <chrono>
#include <ctime>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define LINE_WIDTH 10
//#define BALL_RADIUS 50.f
#define NUMBER_OF_VECTORS 2500
#define NUMBER_OF_VECTORS_PER_ROW 50
#define NUMBER_OF_VECTORS_PER_COLUMN 50
#define FRAME_RATE 200
#define STEP 0.02f
#define res 100
#define MASSFACTOR .001//will need to play with this number (used in collision response)

//extern int main_maxVel;

	class planet {
	public:		
		std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

		sf::Vector2f OrbitPoint = { 0,0 };
		sf::Vector2f Time = { 0,0 };
		sf::Vector2f PosF = { 0,0 };
		sf::Vector2f PosI = { 0,0 };
		sf::Vector2f VelI = { 30,30 };
		sf::Vector2f Vdir = { 0,0 };
		sf::Vector2f PosF_temp = { 0,0 };
		sf::Vector2f VelF = { 0,0 };
		float AvgVel = 10;
		unsigned int ZeroVelocityCount, ZeroVelocityLimit;
		sf::CircleShape* Ball = nullptr;
		float Radius = 0;
		float Mass = 1;
		int overFlag = 0;
		sf::RectangleShape* LineVectors = nullptr;
		std::vector<sf::RectangleShape*> rectangleSet;
		int maxVel = 1;
		float fieldStrength = 0.2f; //strength of the field -------------------(set to zero for collion testing)
		float fieldMod = 2.5; //changes the vector field circular directions
		float orbitDir = 1;
		float wallFriction = 0;
		//this is where i will use the vxField and vyField to rotate each rectangle
		int drawVectorFiledON = 0;
		float commandedAngle[NUMBER_OF_VECTORS] = { 0 };
		float magnitude2, Opp, Adj, error, currentX[NUMBER_OF_VECTORS], currentY[NUMBER_OF_VECTORS];
		int idx = 0;
		

		//returns a random number LOW (inclusive) to HIGH (exclusive)
		float random(int low, int high) {
			//auto duration = now.time_since_epoch();
			//typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<8>>::type> Days; /* UTC: +8:00 */
			//auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
			

			//srand((unsigned int) (nanoseconds.count()));
			float temp = (float)(rand() % res); //temp is 0 to 100
			temp = temp / res; //temp is 0.0 to 1.0
			return (float)((temp * (high - low)) + low);

		}

		planet(sf::Vector2f mousePosBall, int radioactivity = 2, float fieldmag = 0.0f, int drawVectorFiledONOFF = 0);
		//~orbit();
		void update(sf::Vector2f mousePosBall); //update the snake postion and the screen
		void resetPlanet();
		bool isVectInProx(sf::Vector2f,sf::Vector2f, float);
		float getMag(sf::Vector2f vBase, sf::Vector2f vMoving);
		float getMag(sf::Vector2f vBase);
		sf::Vector2f vMultiply(sf::Vector2f vBase, sf::Vector2f vMoving);
	protected:
	
	private:
		
	};

#endif