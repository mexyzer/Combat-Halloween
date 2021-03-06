#pragma once

#include "own/sprite.h"
#include <SFML/Graphics.hpp>
#include "play/game/foe/balloonchat.h"
#include <vector>

class Eye
{
	float screen_w;
	float screen_h;
	
	float line_y;
	float distance_y;
	
	float x, y;
	float scale_x;
	
	MySprite sprite;
	
	// Texts.
	Balloonchat balloonchat;
	vector <string> texts;
	int textWas;
	float textCounter;
	float textLine;
	
public:
	
	Eye();
	~Eye();
	void free();
	void reset( float x, float y );
	
	void load( float screen_w, float screen_h );
	void draw( sf::RenderWindow* &window, sf::Shader &shader );
	void mechanics( double elapsedTime );
	
	// Position.
	void setPosition( float x, float y );
};