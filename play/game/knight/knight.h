#pragma once
#include "own/sprite.h"
#include "own/request.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>
#include <vector>

class Knight
{
	// HP in sprite.
	MySprite bar;
	MySprite table;
	
	// Just for test.
	bool collision;
	sf::RectangleShape rectcollisionwalk;
	sf::RectangleShape rectcollisionattack;
	
	// View.
	float viewX, viewY;
	sf::View view;
	
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
	float offset_line;
	vector <MySprite*> sprites;
	
	// Steer.
	sf::Uint8 flip;
	
	// Features.
	string username;
	float armour;
	float damage;
	float velocity;
	float heartpoints;
	float heartpoints_state;
	float gravity_value;
	
	
	// Activity variables.
	float running;
	float running_line;
	bool jump_released;
	bool jump_key_released;
	int jumping_counter;
	int jumping_line;
	int attack_counter;
	int dead;
	
	
	// Thread.
	string error;
	MyThread thread;
	
public:
	
	// Basics.
	Knight();
	~Knight();
	void free();
	void reset();
	void load( float screen_w, float screen_h );
	void handle( sf::Event& event );
	void draw( sf::RenderWindow* &window, sf::Shader &shader );
	
	// Keyboard - test.
	void turnCollision( bool collision );
	
	// Activities.
	void flipping();
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
	void backjumping( double elapsedTime );
	void weightlessness( double elapsedTime );
	
	
	// Thread.
	bool isNull();
	bool isReady();
	void setThread( string message );
	void setFeatures( string message );
	bool setFeaturesPartOne( string line );
	void setFeaturesPartTwo();
	bool positionKnight( float x, float y, vector <sf::Vector2f> fs, vector <sf::Uint8> types );
	string getError();
	
	// Sound and username.
	void setUsername( string line );
	
	
	// The rest.
	bool isAttacking( bool hide = false );
	void harm( float value );
	float getDamage();
	float getHPScale();
	void commitSuicide();
	bool isDeath();
	bool isRemains();
	
	sf::View& getView();
	float getViewX();
	float getViewY();
	sf::Rect <float> getRect();
	sf::Rect <float> getAttackRect();
	float getX();
	float getY();
	float getLeft();
	float getRight();
	float getWidth();
	float getHeight();
};