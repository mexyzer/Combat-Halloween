#pragma once
#include "own/sprite.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <vector>
#include <thread>

class Knight
{
	float screen_w;
	float screen_h;
	
	sf::View view;
	int viewState;
	
	enum Keys
	{
		LEFT_K = 0,
		RIGHT_K,
		JUMP_K,
		ATTACK_K,
		AMOUNT_K
	};
	vector <int> keys;
	
	
	enum Activity
	{
		IDLE = 0,
		WALK,
		RUN,
		JUMP,
		ATTACK,
		JUMP_ATTACK,
		DEATH,
		AMOUNT
	};
	int line;
	int which;
	float offset;
	vector <MySprite*> sprites;
	
	// Steer.
	sf::Uint8 flip;
	
	// Features.
	string username;
	float armour;
	float damage;
	float velocity;
	float heartpoints;
	
	
	// Activity variables.
	float running;
	float running_line;
	
	bool jump_released;
	bool jump_key_released;
	int jumping_counter;
	int jumping_line;
	
	int dead;
	
	
	// Thread.
	std::thread* myThread;
	bool thread_ready;
	bool ready;
	
public:
	
	// Basics.
	Knight();
	~Knight();
	void free();
	void reset();
	void load( float screen_w, float screen_h );
	void handle( sf::Event& event );
	void draw( sf::RenderWindow* &window );
	void fadein( float v = 1, int max = 0xFF );
	void fadeout( float v = 1, int min = 0 );
	
	// Activities.
	void animation( double elapsedTime );
	void idle( double elapsedTime );
	bool jump();
	bool attack();
	bool moveLeft( double elapsedTime );
	bool moveRight( double elapsedTime );
	void gravity( double elapsedTime );
	void jumping( double elapsedTime );
	
	// Hero.
	void move( float x = 0, float y = 0 );
	void back( double elapsedTime );
	void weightlessness( double elapsedTime );
	
	
	// Thread.
	bool isNull();
	bool isReady();
	void setThread();
	void setFeatures();
	
	// Sound and username.
	void setUsername( string line );
	
	
	// The rest.
	void commitSuicide();
	bool isDeath();
	bool isRemains();
	
	sf::View& getView();
	void setViewState( float left, float right );
	
	sf::Rect <float> getRect();
	float getX();
	float getY();
	float getLeft();
	float getRight();
	float getWidth();
	float getHeight();
};