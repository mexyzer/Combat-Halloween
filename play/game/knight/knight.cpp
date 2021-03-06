#include "knight.h"
#include "own/file.h"
#include <SFML/Window/Keyboard.hpp>

Knight::Knight()
{
	free();
}

Knight::~Knight()
{
	free();
}

void Knight::free()
{
	bar.free();
	table.free();
	
	collision = false;
	
	viewX = viewY = 0;
	
	if( !keys.empty() )
	{
		keys.clear();
	}
	
	line = 0;
	which = IDLE;
	offset = 0;
	offset_line = 0;
	if( !sprites.empty() )
	{
		for( auto &it :sprites )
		{
			it->free();
			delete it;
			it = NULL;
		}
		
		sprites.clear();
	}
	
	flip = 0;
	
	username = "adriqun";
	armour = 0;
	damage = 0;
	velocity = 0;
	heartpoints = 0;
	heartpoints_state = 0;
	gravity_value = 0;
	
	
	running = 0;
	running_line = 0;
	jump_released = true;
	jump_key_released = true;
	jumping_counter = 0;
	jumping_line = 0;
	attack_counter = 0;
	dead = 0;
	
	error = "";
	thread.free();
}

void Knight::reset()
{
	collision = false;
	
	if( !keys.empty() )
	{
		keys.clear();
	}
	
	which = IDLE;
	offset = 0;
	
	armour = 0;
	damage = 10;
	velocity = 100;
	heartpoints = 100;
	heartpoints_state = heartpoints;
	gravity_value = velocity;
	
	
	running = 0;
	running_line = 0.5;
	jump_released = true;
	jump_key_released = true;
	jumping_counter = 0;
	jumping_line = 2;
	attack_counter = 0;
	dead = 0;
	
	error = "";
	thread.free();
}

void Knight::load( float screen_w, float screen_h )
{
	free();
	
	bar.setIdentity( "knight-bar" );
	bar.load( "images/play/foes/foebar.png" );
	bar.setScale( 0.5, 0.5 );
	
	table.setIdentity( "knight-table" );
	table.load( "images/play/foes/foetable.png" );
	table.setScale( 0.5, 0.5 );
	
	rectcollisionwalk.setFillColor( sf::Color( 0xFF, 0xFF, 0xFF, 0xAA ) );
	rectcollisionattack.setFillColor( sf::Color( 0xFF, 0xAE, 0x19, 0xAA ) );
	
	view.setSize( screen_w, screen_h );
	
	line = 10;
	for( unsigned i = 0; i < AMOUNT; i++ )
	{
		sprites.push_back( new MySprite() );
		sprites[ i ]->setIdentity( "knight-sprites" );
		sprites[ i ]->load( "images/play/knight/" +con::itos( i ) +".png", line );
		sprites[ i ]->setScale( 0.5, 0.5 );
	}
	
	// Features and other variables.
	reset();
}

void Knight::handle( sf::Event& event )
{
	if( event.type == sf::Event::KeyReleased )
	{
		if( event.key.code == keys[ JUMP_K ] )
		{
			jump_key_released = true;
		}
	}
}

void Knight::draw( sf::RenderWindow* &window, sf::Shader &shader )
{
	sprites[ which ]->setOffset( offset );
	window->draw( sprites[ which ]->get(), &shader );
	
	if( collision )
	{
		rectcollisionwalk.setSize( sf::Vector2f( getRect().width, getRect().height ) );
		rectcollisionwalk.setPosition( sf::Vector2f( getRect().left, getRect().top ) );
		window->draw( rectcollisionwalk, &shader );
		
		if( isAttacking( true ) )
		{
			rectcollisionattack.setSize( sf::Vector2f( getAttackRect().width, getAttackRect().height ) );
			rectcollisionattack.setPosition( sf::Vector2f( getAttackRect().left, getAttackRect().top ) );
			window->draw( rectcollisionattack, &shader );
		}
	}
	
	// Info.
	table.setPosition( getRect().left +getRect().width/2 -table.getWidth()/2, getRect().top -table.getHeight() *1.5 );
	window->draw( table.get(), &shader );
	
	bar.setScale( getHPScale() *0.5, 0.5 );
	bar.setPosition( table.getX(), table.getY() );
	shader.setUniform( "hiddenColor", sf::Glsl::Vec4( (0xFF -(armour*1.25)) /0xFF, 0.0, 0.0, 1.0 ) );
	window->draw( bar.get(), &shader );
	shader.setUniform( "hiddenColor", sf::Glsl::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
}

void Knight::turnCollision( bool collision )
{
	this->collision = collision;
}



void Knight::flipping()
{
	// If flip.
	if( flip == 2 )
	{
		for( auto &it :sprites )
		{
			it->move( -it->getWidth(), 0 );
		}
		
		for( auto &it :sprites )
		{
			it->flipHorizontally();
		}
		
		flip = 0;
	}
	else if( flip == 3 )
	{
		for( auto &it :sprites )
		{
			it->move( it->getWidth(), 0 );
		}
		
		for( auto &it :sprites )
		{
			it->flipHorizontally();
		}
		
		flip = 1;
	}
}

void Knight::animation( double elapsedTime )
{
	if( dead < 2 )
	{
		// Animation.
		offset += elapsedTime *offset_line;
		if( offset >= line )
		{
			if( which == DEATH )
			{
				dead = 2;
			}
			else
			{
				offset = 0;
				if( which == JUMP || which == ATTACK || which == JUMP_ATTACK )
				{
					which = IDLE;
				}
			}
		}
		
		// Jump mechanics.
		if( offset > line /2 )
		{
			jump_released = true;
		}
		
		// If y.
		if( getY() > view.getSize().y )
		{
			if( flip != 0 )
			{
				flip = 2;
			}
		}
		
		flipping();

		
		// If y cd.
		if( getY() > view.getSize().y )
		{
			for( auto &it :sprites )
			{
				it->setPosition( view.getSize().x /128, view.getSize().y -(sprites[ IDLE ]->getWidth()*2) +(sprites[ IDLE ]->getWidth() -it->getWidth()) );
			}
		}
		
		// Set view.
		float velocity = 1280 /view.getSize().x;
		if( getX() +getWidth()/2 > view.getSize().x/2 || viewX > view.getSize().x/2 )
		{
			if( viewX < getX() +getWidth()/2 )
			{
				viewX += elapsedTime *(getX() +getWidth()/2 -viewX) *velocity;
			}
			else if( viewX > getX() +getWidth()/2 )
			{
				viewX -= elapsedTime *(viewX -getX() +getWidth()/2) *velocity;
			}
		}
		else
		{
			viewX = view.getSize().x /2;
		}
		
		if( getY() +getHeight()/2 < view.getSize().y /2 || viewY < view.getSize().y/2 )
		{
			if( viewY < getY() +getHeight()/2 )
			{
				viewY += elapsedTime *(getY() +getHeight()/2 -viewY) *velocity;
			}
			else if( viewY > getY() +getHeight()/2 )
			{
				viewY -= elapsedTime *(viewY -getY() +getHeight()/2) *velocity;
			}
		}
		else
		{
			viewY = view.getSize().y /2;
		}
		
		view.setCenter( sf::Vector2f( viewX, viewY ) );
	}
}

void Knight::idle( double elapsedTime )
{
	if( which != JUMP && which != ATTACK && which != JUMP_ATTACK )
	{
		which = IDLE;
	}
	
	if( running > -running_line/2 )
	{
		running -= elapsedTime;
	}
}

bool Knight::jump()
{
	if( jumping_counter < jumping_line && jump_key_released )
	{
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Key( keys[ JUMP_K ] ) ) )
		{
			if( jump_released )
			{
				offset = 0;
				which = JUMP;
				jumping_counter ++;
			}
			
			jump_released = false;
			jump_key_released = false;
			return true;
		}
	}
	
	return false;
}

bool Knight::attack()
{
	if( which != ATTACK && which != JUMP_ATTACK )
	{
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Key( keys[ ATTACK_K ] ) ) )
		{
			attack_counter = 0;
			if( which == JUMP )
			{
				offset = 0;
				which = JUMP_ATTACK;
			}
			else
			{
				offset = 0;
				which = ATTACK;
			}
			
			return true;
		}
	}
	
	return false;
}

bool Knight::moveLeft( double elapsedTime )
{
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::Key( keys[ LEFT_K ] ) ) )
	{
		if( which != JUMP && which != ATTACK && which != JUMP_ATTACK )
		{
			if( running > running_line )
			{
				which = RUN;
			}
			else
			{
				which = WALK;
			}
		}
		
		int multiplier = 1;
		if( running > running_line )	multiplier ++;
		if( which == JUMP )				multiplier ++;
		
		if( flip != 1 )
		{
			flip = 3;
		}
		if( running < running_line *1.5 )
		{
			running += elapsedTime;
		}
		
		if( which != ATTACK )
		{
			move( -elapsedTime *velocity *multiplier );
		}
		
		return true;
	}
	
	return false;
}

bool Knight::moveRight( double elapsedTime )
{
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::Key( keys[ RIGHT_K ] ) ) )
	{
		if( which != JUMP && which != ATTACK && which != JUMP_ATTACK )
		{
			if( running > running_line )
			{
				which = RUN;
			}
			else
			{
				which = WALK;
			}
		}
		
		int multiplier = 1;
		if( running > running_line )	multiplier ++;
		if( which == JUMP )				multiplier ++;
		
		if( flip != 0 )
		{
			flip = 2;
		}
		
		if( running < running_line *1.5 )
		{
			running += elapsedTime;
		}
		
		if( which != ATTACK )
		{
			move( elapsedTime *velocity *multiplier );
		}
		
		return true;
	}
	
	return false;
}

void Knight::gravity( double elapsedTime )
{
	if( which == JUMP )
	{
		move( 0, elapsedTime *gravity_value *2 );
	}
	else if( which != JUMP_ATTACK )
	{
		move( 0, elapsedTime *gravity_value *3 );
	}
}

void Knight::jumping( double elapsedTime )
{
	if( (which == JUMP) && offset < line /2 )
	{
		move( 0, -elapsedTime *velocity *6 );
	}
}



void Knight::move( float x, float y )
{
	for( unsigned i = 0; i < sprites.size(); i++ )
	{
		sprites[ i ]->setPosition( sprites[ i ]->getX() +x, sprites[ i ]->getY() +y );
	}
}

void Knight::back( double elapsedTime )
{
	int multiplier = 1;
	if( running > running_line )	multiplier ++;
	if( which == JUMP )				multiplier ++;
	if( flip == 0 || flip == 2 )	multiplier = -multiplier;
	
	move( elapsedTime *velocity *multiplier );
}

void Knight::backjumping( double elapsedTime )
{
	move( 0, elapsedTime *velocity *6 );
}

void Knight::weightlessness( double elapsedTime )
{
	gravity( -elapsedTime );
	
	if( which != JUMP )
	{
		jumping_counter = 0;
	}
}



bool Knight::isReady()
{
	return thread.s;
}

bool Knight::isNull()
{
	// Delete thread.
	if( thread.t != NULL && thread.r )
	{
		thread.reset();
	}
	
	if( thread.t == NULL )
	{
		return true;
	}
	
	return false;
}

void Knight::setThread( string message )
{
	if( !thread.s )
	{
		if( !thread.r && thread.t == NULL )
		{
			thread.t = new std::thread( [=] { setFeatures( message ); } );
			thread.t->detach();
		}
	}
}

void Knight::setFeatures( string message )
{
	if( setFeaturesPartOne( message ) )
	{
		setFeaturesPartTwo();
	}
	
	// Inform that thread is ready for next action.
	thread.r = true;
}

bool Knight::setFeaturesPartOne( string line )
{
	// Bufor and start.
	string bufor = "";
	unsigned start = 0;
	
	
	// AUTHOR --------------------------------------------------------------------------
	for( unsigned i = start; i < line.size(); i++ )
	{
		if( line[ i ] == '|' )
		{
			start = i +1;
			// printf( "%s\n", bufor.c_str() );
			bufor = "";
			break;
		}
		
		bufor += line[ i ];
	}
	
	
	// NEW SIZES --------------------------------------------------------------------------
	float my_screen_w = 0;
	float my_screen_h = 0;
	
	// Set my_screen_w.
	for( unsigned i = start; i < line.size(); i++ )
	{
		if( line[ i ] == '|' )
		{
			start = i +1;
			my_screen_w = view.getSize().x -con::stof( bufor );
			bufor = "";
			break;
		}
		
		bufor += line[ i ];
	}
	
	// Set my_screen_h.
	for( unsigned i = start; i < line.size(); i++ )
	{
		if( line[ i ] == '|' )
		{
			start = i +1;
			my_screen_h = view.getSize().y -con::stof( bufor ) +1;
			bufor = "";
			break;
		}
		
		bufor += line[ i ];
	}
	
	vector <sf::Vector2f> fs_tiles;
	vector <sf::Uint8> types_tiles;
	float myKnightX = 0;
	float myKnightY = 0;
	// FS --------------------------------------------------------------------------
	for( unsigned i = start; i < line.size(); i++ )
	{
		if( line[ i ] == '|' )
		{
			bufor += "*";
			string nrstr = "";
			vector <string> data;
			
			bool wrong = false;
			for( unsigned j = 0; j < bufor.size(); j++ )
			{
				if( bufor[ j ] == '*' )
				{
					if( data.size() == 0 )
					{
						if( con::stoi( nrstr ) != 0 && con::stoi( nrstr ) != 1 )
						{
							wrong = true;
							break;
						}
					}
					
					data.push_back( nrstr );
					nrstr = "";
				}
				else
				{
					nrstr += bufor[ j ];
				}
			}
			
			if( !wrong )
			{
				sf::Uint8 w = con::stoi( data[ 0 ] );
				sf::Uint8 t = con::stoi( data[ 1 ] );
				float x = con::stoi( data[ 2 ] ) *0.995;
				float y = con::stoi( data[ 3 ] ) +my_screen_h;
				
				// Set knight position
				if( w == 0 )
				{
					myKnightX = x;
					myKnightY = y;
				}
				else if( w == 1 )
				{
					fs_tiles.push_back( sf::Vector2f( x, y ) );
					types_tiles.push_back( t );
				}
			}
			
			// Clear.
			bufor = "";
		}
		else
		{
			bufor += line[ i ];
		}
	}
	
	return positionKnight( myKnightX, myKnightY, fs_tiles, types_tiles );
}

void Knight::setFeaturesPartTwo()
{
	MyRequest request;
	request.setMessage( "username=" +username );
	request.setHttp( "http://adrianmichalek.pl/" );
	request.setRequest( "/combathalloween/getparts.php", sf::Http::Request::Post );
	
	if( request.sendRequest() )
	{
		string result = request.getResult();
		if( result != "0" )	// success
		{
			float armour_temporary = 0;
			float damage_temporary = 0;
			float velocity_temporary = 0;
			float heartpoints_temporary = 0;
			
			for( unsigned i = 0; i < result.size(); i++ )
			{
				if( result[ i ] == 'c' )
				{
					for( unsigned j = i +1; j < result.size(); j++ )
					{
						if( result[ j ] == 'a' )
						{
							i = j -1;
							break;
						}
					}
				}
				
				else if( result[ i ] == 'a' || result[ i ] == 'h' || 
						 result[ i ] == 'd' || result[ i ] == 's' )
				{
					int was = i;
					string value = "";
					for( unsigned j = i +1; j < result.size(); j++ )
					{
						if( isalpha( result[ j ] ) )
						{
							i = j -1;
							break;
						}
						else
						{
							value += result[ j ];
						}
					}
					
					// printf( "%f\n", con::stof( value ) );
					switch( result[ was ] )
					{
						case 'a': armour_temporary += 		con::stof( value );	break;
						case 'h': heartpoints_temporary += 	con::stof( value );	break;
						case 'd': damage_temporary += 		con::stof( value );	break;
						case 's': velocity_temporary +=		con::stof( value );	break;
					}
				}
			}
			
			armour = armour +(armour*armour_temporary /100);
			damage = damage +(damage*damage_temporary /100);
			velocity = velocity +(velocity*velocity_temporary /100);
			heartpoints = heartpoints +(heartpoints*heartpoints_temporary /100);
			heartpoints_state = heartpoints;
			
			if( velocity < 0 )
			{
				velocity = 5;
			}
			
			offset_line = velocity /4;
			if( offset_line < 20 )
			{
				offset_line = 20;
			}
			else if( offset_line > 40 )
			{
				offset_line = 40;
			}
			
			// printf( "1. a: %f  h: %f  d: %f  s: %f\n", armour_temporary, heartpoints_temporary, damage_temporary, velocity_temporary );
			// printf( "2. a: %f  h: %f  d: %f  s: %f\n", armour, heartpoints, damage, velocity );
			
			// Set keys.
			MyFile file;
			file.load( "txt/keys_current.txt" );
			if( file.is_good() )
			{
				string line;
				for( unsigned i = 0; i < AMOUNT_K; i++ )
				{
					getline( file.get(), line );
					keys.push_back( con::stoi( line ) );
				}
				
				thread.s = true;
			}
			else
			{
				error = "Can't load txt/keys_current.txt";
			}
			
			file.free();
		}
	}
	else
	{
		error = "Can't correctly set character.";
	}
	
}

bool Knight::positionKnight( float x, float y, vector <sf::Vector2f> fs, vector <sf::Uint8> types )
{
	
	// Setting position of knight...
	MySprite block;
	block.setIdentity( "knight-block" );
	block.load( "images/play/tiles/0.png" );
	block.setScale( 0.5, 0.5 );

	float block_x = 0;
	float block_y = 0;
	int success = -1;
	
	float knight_x = x;
	float knight_y = y +sprites[ IDLE ]->getHeight();
	
	for( unsigned j = 0; j < fs.size(); j++ )
	{
		if( types[ j ] >= 0 && types[ j ] <= 8 )
		{
			block_x = fs[ j ].x;
			block_y = fs[ j ].y;
			
			if( knight_x > block_x && knight_x < block_x +block.getWidth() )
			{
				if( knight_y > block_y && knight_y < block_y +block.getHeight() )
				{
					success = j;
					break;
				}
			}
		}
	}
	
	if( success < 0 )	// Wrong put knight.
	{
		printf( "%d %d\n", fs.size(), types.size() );
		error = "The position of character is wrong.";
		return false;
	}
	else
	{
		block_y -= sprites[ IDLE ]->getHeight() *1.1;
		// printf( "%f  %f\n", knight_x, block_y );
		
		viewX = knight_x;
		viewY = block_y;
		view.setCenter( viewX, viewY );
		for( auto &it :sprites )
		{
			it->setPosition( knight_x, block_y +sprites[ IDLE ]->getWidth() -it->getWidth() );
		}
		
		if( flip != 0 )
		{
			flip = 2;
			flipping();
		}
	}
	
	block.free();
	
	return true;
}

string Knight::getError()
{
	return error;
}



void Knight::setUsername( string line )
{
	this->username = line;
}


bool Knight::isAttacking( bool hide )
{
	if( which == ATTACK && static_cast <int> (offset) == 4 )
	{
		if( hide )
		{
			return true;
		}
		else if( attack_counter == 0 )
		{
			attack_counter = 1;
			return true;
		}
	}
	else if( which == JUMP_ATTACK && static_cast <int> (offset) == 5 )
	{
		if( hide )
		{
			return true;
		}
		else if( attack_counter == 0 )
		{
			attack_counter = 1;
			return true;
		}
	}
	
	return false;
}

void Knight::harm( float value )
{
	heartpoints -= (value -(value *armour /1000));
	if( heartpoints <= 0 )
	{
		commitSuicide();
	}
}

float Knight::getDamage()
{
	return damage;
}

float Knight::getHPScale()
{
	if( heartpoints /heartpoints_state < 0 )
	{
		return 0;
	}
	
	return heartpoints /heartpoints_state;
}

void Knight::commitSuicide()
{
	if( dead == 0 )
	{
		dead = 1;
		which = DEATH;
		offset = 0;
		
		if( flip == 0 || flip == 2 )
		{
			sprites[ DEATH ]->setPosition( sprites[ IDLE ]->getX() -(sprites[ DEATH ]->getWidth() -sprites[ IDLE ]->getWidth()), sprites[ IDLE ]->getY() );
		}
		else
		{
			sprites[ DEATH ]->setPosition( sprites[ IDLE ]->getX() +(sprites[ DEATH ]->getWidth() -sprites[ IDLE ]->getWidth()), sprites[ IDLE ]->getY() );
		}
	}
}

bool Knight::isDeath()
{
	if( dead > 0 )
	{
		return true;
	}
	
	return false;
}

bool Knight::isRemains()
{
	if( dead == 2 )
	{
		return true;
	}
	
	return false;
}



sf::View& Knight::getView()
{
	return view;
}

float Knight::getViewX()
{
	return view.getCenter().x -view.getSize().x/2;
}

float Knight::getViewY()
{
	return view.getCenter().y -view.getSize().y/2;
}

sf::Rect <float> Knight::getRect()
{
	sf::Rect <float> rect;
	
	rect.left = getX();
	rect.top = getY();
	rect.width = getWidth();
	rect.height = getHeight() *0.99;
	
	return rect;
}

sf::Rect <float> Knight::getAttackRect()
{
	sf::Rect <float> rect;
	
	rect.left = getX();
	
	if( flip == 0 || flip == 2 )
	{
		rect.left += getWidth()/2;
	}
	else
	{
		rect.left -= getWidth()/10;
	}
	
	rect.width = getWidth() /5 *3;
	rect.height = getHeight() /5 *3;
	rect.top = getY() +(getHeight() /5 *2);
	
	return rect;
}

float Knight::getX()
{
	float x = sprites[ which ]->getX();
	
	if( flip == 1 || flip == 3 )
	{
		x -= (getWidth() +sprites[ which ]->getWidth() /6);
	}
	else
	{
		x += sprites[ which ]->getWidth() /6;
	}
	
	return x;
}

float Knight::getY()
{
	return sprites[ IDLE ]->getY();
}

float Knight::getLeft()
{
	return getX();
}

float Knight::getRight()
{
	return getX() +getWidth();
}

float Knight::getWidth()
{
	return sprites[ which ]->getWidth() /3 *2;
}

float Knight::getHeight()
{
	return sprites[ IDLE ]->getHeight();
}