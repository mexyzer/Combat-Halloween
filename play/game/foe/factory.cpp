#include "factory.h"
#include "own/file.h"

template <typename F>
Factory <F>::Factory()
{
	free();
}

template <typename F>
Factory <F>::~Factory()
{
	free();
}

template <typename F>
void Factory <F>::free()
{
	bar.free();
	table.free();
	
	balloonchat.free();
	
	type = -1;
	name = "";
	
	screen_w = 0;
	screen_h = 0;
	
	reset();
}

template <typename F>
void Factory <F>::reset()
{
	border_x = 0;
	border_y = 0;
	collision = false;
	
	thread.free();
	error = "";
	
	damaged = -1;
	
	if( !foes.empty() )
	{
		for( auto &it :foes )
		{
			it->free();
			delete it;
			it = NULL;
		}
		
		foes.clear();
	}
	
	if( !lines.empty() )
	{
		lines.clear();
	}
	
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
}

template <typename F>
void Factory <F>::load( float screen_w, float screen_h, int type, string name )
{
	free();
	
	bar.setIdentity( "factory-bar" );
	bar.load( "images/play/foes/foebar.png" );
	bar.setScale( 0.5, 0.5 );
	
	table.setIdentity( "factory-table" );
	table.load( "images/play/foes/foetable.png" );
	table.setScale( 0.5, 0.5 );
	
	balloonchat.load( screen_w, screen_h );
	
	rectcollisionwalk.setFillColor( sf::Color( 0xFF, 0x40, 0xFF, 0xAA ) );
	rectcollisionattack.setFillColor( sf::Color( 0xCC, 0, 0, 0xAA ) );
	
	this->type = type;
	this->name = name;
	this->screen_w = screen_w;
	this->screen_h = screen_h;
}

template <typename F>
void Factory <F>::draw( sf::RenderWindow* &window, sf::Shader &shader )
{
	float x;
	float y;
	int t;
	
	for( unsigned i = 0; i < foes.size(); i++ )
	{
		t = foes[ i ]->getState();
		if( t < 0 )
		{
			continue;
		}
		
		sprites[ t ]->setScale( foes[ i ]->getScaleX(), foes[ i ]->getScaleY() );
		x = foes[ i ]->getX();
		y = foes[ i ]->getY() -sprites[ t ]->getHeight();
		
		
		if( x < border_x +screen_w && y -sprites[ t ]->getHeight() < border_y +screen_h )
		{
			if( x +sprites[ t ]->getWidth() > border_x && y +sprites[ t ]->getHeight() > border_y )
			{
				sprites[ t ]->setOffset( foes[ i ]->getOffset() );
				sprites[ t ]->setPosition( x, y );
				window->draw( sprites[ t ]->get(), &shader );
				
				// Just for test.
				if( collision )
				{
					rectcollisionwalk.setSize( sf::Vector2f( foes[ i ]->getRealWidth(), foes[ i ]->getRealHeight() ) );
					rectcollisionwalk.setPosition( sf::Vector2f( foes[ i ]->getRealX(), foes[ i ]->getRealY() ) );
					window->draw( rectcollisionwalk, &shader );
					
					if( foes[ i ]->isAttacking( true ) )
					{
						rectcollisionattack.setSize( sf::Vector2f( foes[ i ]->getAttackWidth(), foes[ i ]->getAttackHeight() ) );
						rectcollisionattack.setPosition( sf::Vector2f( foes[ i ]->getAttackX(), foes[ i ]->getAttackY() ) );
						window->draw( rectcollisionattack, &shader );
					}
				}
				
				float scale = foes[ i ]->getScaleY();
				
				// Info.
				table.setScale( scale, scale );
				table.setPosition( foes[ i ]->getRealX() +foes[ i ]->getRealWidth()/2 -table.getWidth()/2, y -table.getHeight() *1.5 );
				window->draw( table.get(), &shader );
				
				bar.setScale( foes[ i ]->getHPScale() *scale, scale );
				bar.setPosition( table.getX(), table.getY() );
				shader.setUniform( "hiddenColor", sf::Glsl::Vec4( (0xFF -(foes[ i ]->getArmour()/5)) /0xFF, 0.0, 0.0, 1.0 ) );
				window->draw( bar.get(), &shader );
				shader.setUniform( "hiddenColor", sf::Glsl::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
				
				// Comments from foes.
				if( foes[ i ]->showText() )
				{
					balloonchat.setText( foes[ i ]->getText() );
					balloonchat.setPosition( foes[ i ]->getMouthX(), foes[ i ]->getMouthY(), foes[i ]->isLeftText() );
					balloonchat.draw( window, shader );
				}
			}
		}
	}
}

template <typename F>
void Factory <F>::mechanics( double elapsedTime )
{
	float x;
	float y;
	int t;
	
	for( unsigned i = 0; i < foes.size(); i++ )
	{
		t = foes[ i ]->getState();
		if( t < 0 )
		{
			continue;
		}
		
		sprites[ t ]->setScale( foes[ i ]->getScaleX(), foes[ i ]->getScaleY() );
		x = foes[ i ]->getX();
		y = foes[ i ]->getY() -sprites[ t ]->getHeight();
		
		if( x < border_x +screen_w && y < border_y +screen_h )
		{
			if( x +sprites[ t ]->getWidth() > border_x && y +sprites[ t ]->getHeight() > border_y )
			{
				foes[ i ]->mechanics( elapsedTime );
			}
		}
	}
}



template <typename F>
void Factory <F>::turnCollision( bool collision )
{
	this->collision = collision;
}

template <typename F>
bool Factory <F>::getCollision()
{
	return collision;
}




template <typename F>
bool Factory <F>::isNull()
{
	// Delete thread.
	if( thread.t != NULL && thread.r )
	{
		thread.reset();
		return true;
	}
	
	if( thread.t == NULL )
	{
		return true;
	}
	
	return false;
}

template <typename F>
bool Factory <F>::isReady()
{
	return thread.s;
}

template <typename F>
void Factory <F>::setThread( string message )
{
	if( !thread.s )
	{
		if( !thread.r && thread.t == NULL )
		{
			thread.t = new std::thread( [=] { prepare( message ); } );
			thread.t->detach();
		}
	}
}

template <typename F>
void Factory <F>::prepare( string message )
{
	// Bufor and start.
	string bufor = "";
	unsigned start = 0;
	
	string line = message;
	
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
	// float my_screen_w = 0;
	float my_screen_h = 0;
	
	// Set my_screen_w.
	for( unsigned i = start; i < line.size(); i++ )
	{
		if( line[ i ] == '|' )
		{
			start = i +1;
			// my_screen_w = screen_w -con::stof( bufor );
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
			my_screen_h = screen_h -con::stof( bufor ) +1;
			bufor = "";
			break;
		}
		
		bufor += line[ i ];
	}
	
	vector <sf::Vector2f> fs_tiles;
	vector <sf::Uint8> types_tiles;
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
						if( con::stoi( nrstr ) != 1 && con::stoi( nrstr ) != 5 )
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
				
				if( w == 5 )
				{
					if( t == type )
					{
						// Features.
						float myarmour = con::stoi( data[ 4 ] );
						float mydamage = con::stoi( data[ 5 ] );
						float myvelocity = con::stoi( data[ 6 ] );
						float myheartpoints = con::stoi( data[ 7 ] );
						float myscale = con::stof( data[ 8 ] );
						
						foes.push_back( new F );
						foes[ foes.size() -1 ]->setPosition( x, y );
						foes[ foes.size() -1 ]->setCenterX( x );
						foes[ foes.size() -1 ]->setArmour( myarmour );
						foes[ foes.size() -1 ]->setDamage( mydamage );
						foes[ foes.size() -1 ]->setVelocity( myvelocity );
						foes[ foes.size() -1 ]->setHeartpoints( myheartpoints );
						foes[ foes.size() -1 ]->setScale( myscale );
						
						for( unsigned i = 9; i < data.size(); i++ )
						{
							for( unsigned k = 0; k < data[ i ].size(); k++ )
							{
								if( data[ i ][ k ] == '~' )
								{
									data[ i ][ k ] = '\n';
								}
							}
							
							foes[ foes.size() -1 ]->addText( data[ i ] );
						}
					}
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
	
	MyRequest request;
	request.setMessage( "name=" +name );
	request.setHttp( "http://adrianmichalek.pl/" );
	request.setRequest( "/combathalloween/getline.php", sf::Http::Request::Post );
	
	if( request.sendRequest() )
	{
		string result = request.getResult();
		if( result != "0" )	// success
		{
			string line = "";
			for( unsigned i = 0; i < result.size(); i++ )
			{
				if( result[ i ] == ',' || result[ i ] == '.' )
				{
					lines.push_back( con::stoi( line ) );
					line = "";
					
					if( result[ i ] == '.' )
					{
						break;
					}
				}
				else
				{
					line += result[ i ];
				}
			}
			
			// Load sprites.
			for( unsigned i = 0; i < lines.size(); i++ )
			{
				sprites.push_back( new MySprite() );
				sprites[ i ]->setIdentity( "factory-sprites" );
				sprites[ i ]->load( "images/play/foes/" +name +"/" +con::itos(i) +".png", lines[ i ] );
				sprites[ i ]->setScale( 0.5, 0.5 );
				// printf( "%d\n", lines[ i ] );
			}
			
			for( auto &it :foes )
			{
				it->setLines( lines );
				it->setWidth( sprites[ 0 ]->getWidth()*it->getScaleX()*2 );
			}
			
			positioning( fs_tiles, types_tiles );
			
			// Inform that everything is ok.
			thread.s = true;
		}
	}
	else
	{
		error = "Cannot download foe settings.";
	}
	
	fs_tiles.clear();
	types_tiles.clear();
	
	// Inform that thread is ready for next action.
	thread.r = true;
}

template <typename F>
void Factory <F>::positioning( vector <sf::Vector2f> fs, vector <sf::Uint8> types )
{
	// Setting left and right of particular foe...
	MySprite block;
	block.setIdentity( "foes-block" );
	block.load( "images/play/tiles/0.png" );
	block.setScale( 0.5, 0.5 );

	float foe_x = 0;
	float foe_y = 0;
	float block_x = 0;
	float block_y = 0;
	int success = -1;
	
	for( int i = 0; i < static_cast <int> (foes.size()); i++ )
	{
		sprites[ 1 ]->setScale( foes[ i ]->getScaleX(), foes[ i ]->getScaleY() );
		
		foe_x = foes[ i ]->getX();
		foe_y = foes[ i ]->getY() +sprites[ 1 ]->getHeight();
		success = -1;
		
		for( unsigned j = 0; j < fs.size(); j++ )
		{
			if( types[ j ] >= 0 && types[ j ] <= 8 )
			{
				block_x = fs[ j ].x;
				block_y = fs[ j ].y;
				
				if( foe_x > block_x && foe_x < block_x +block.getWidth() )
				{
					if( foe_y > block_y && foe_y < block_y +block.getHeight() )
					{
						// printf( "fx:%f fy:%f  bx:%f by:%f bw:%f\n", foe_x, foe_y, block_x, block_y, block.getWidth() );
						success = j;
						break;
					}
				}
			}
		}
		
		if( success < 0 )	// Wrong put foe.
		{
			foes.erase( foes.begin() +i );
			i --;
		}
		else
		{
			
			float good_left = block_x;
			float good_right = block_x +block.getWidth();
			
			// Building path for foe, start with left.
			for( int k = success -1; k >= 0; k-- )
			{
				if( fs[ k ].y == block_y )
				{
					if( types[ k ] >= 0 && types[ k ] <= 8 )
					{
						float error_margin  = good_left -block.getWidth() -fs[ k ].x;
						if( error_margin < 0 )	error_margin = -error_margin;
						
						if( error_margin /block.getWidth() < 0.25 )
						{
							good_left -= block.getWidth();
						}
					}
				}
			}
			
			// right
			for( unsigned k = success +1; k < fs.size(); k++ )
			{
				if( fs[ k ].y == block_y )
				{
					if( types[ k ] >= 0 && types[ k ] <= 8 )
					{
						float error_margin  = fs[ k ].x -good_right;
						if( error_margin < 0 )	error_margin = -error_margin;
						
						if( error_margin /block.getWidth() < 0.25 )
						{
							good_right += block.getWidth();
						}
					}
				}
			}
			
			// printf( "%f %f\n", good_left, good_right );
			foes[ i ]->setBorders( good_left, good_right );
			foes[ i ]->setPosition( foes[ i ]->getCenterX(), block_y );
			
			// delete if surface is too small
			if( good_right -good_left < block.getWidth() *2 )
			{
				foes.erase( foes.begin() +i );
				i --;
			}
		}
	}

	block.free();
}

template <typename F>
string Factory <F>::getError()
{
	return error;
}



template <typename F>
void Factory <F>::setBorderX( float x )
{
	border_x = x;
}

template <typename F>
void Factory <F>::setBorderY( float y )
{
	border_y = y;
}


template <typename F>
void Factory <F>::harm( sf::Rect <float> rect, float damage )
{
	float l;
	float r;
	float rx;
	float ry;
	float rw;
	float rh;
	
	for( unsigned i = 0; i < foes.size(); i++ )
	{
		if( foes[ i ]->isAlive() )
		{
			l = foes[ i ]->getLeft();
			r = foes[ i ]->getRight();
			rx = foes[ i ]->getRealX();
			ry = foes[ i ]->getRealY();
			rw = foes[ i ]->getRealWidth();
			rh = foes[ i ]->getRealHeight();
			
			if( rect.left +rect.width > l && rect.left < r )
			{
				if( rect.top < ry +rh && rect.top +rect.height > ry )
				{
					if( rect.top + rect.height > ry && rect.top < ry +rh )
					{
						if( rect.left + rect.width > rx && rect.left < rx +rw )
						{
							// make damage :/
							foes[ i ]->harm( damage );
						}
					}
				}
			}
		}
	}
}

template <typename F>
bool Factory <F>::isHarmed( sf::Rect <float> rect )
{
	float x, y, w, h;
	for( unsigned i = 0; i < foes.size(); i++ )
	{
		if( foes[ i ]->isAttacking() )
		{
			x = foes[ i ]->getAttackX();
			y = foes[ i ]->getAttackY();
			w = foes[ i ]->getAttackWidth();
			h = foes[ i ]->getAttackHeight();
			
			if( rect.top + rect.height > y && rect.top < y +h )
			{
				if( rect.left + rect.width > x && rect.left < x +w )
				{
					damaged = i;
					return true;
				}
			}
		}
	}
	
	return false;
}

template <typename F>
void Factory <F>::walk( sf::Rect <float> rect, double elapsedTime )
{
	float l;
	float r;
	float rx;
	float ry;
	float rw;
	float rh;
	
	for( unsigned i = 0; i < foes.size(); i++ )
	{
		if( foes[ i ]->isAlive() )
		{
			foes[ i ]->setIdle();
			
			l = foes[ i ]->getLeft();
			r = foes[ i ]->getRight();
			rx = foes[ i ]->getRealX();
			ry = foes[ i ]->getRealY();
			rw = foes[ i ]->getRealWidth();
			rh = foes[ i ]->getRealHeight();
			
			if( rect.left +rect.width > l && rect.left < r )
			{
				if( rect.top < ry +rh && rect.top +rect.height > ry )
				{
					if( foes[ i ]->ableToAppear() )
					{
						if( rx +rw/2 < rect.left +rect.width/2 )	foes[ i ]->turnRight();
						else										foes[ i ]->turnLeft();
						
						foes[ i ]->setAppear();
					}
					else if( foes[ i ]->ableToWalk() )
					{
						if( rx +rw/2 < rect.left +rect.width/2 )	foes[ i ]->turnRight();
						else										foes[ i ]->turnLeft();
						
						foes[ i ]->setWalk();
						
						if( rx +rw < r && rx +rw < rect.left && foes[ i ]->getScaleX() < 0 )
						{
							foes[ i ]->moveX( elapsedTime );
						}
						else if( rx > l && rx > rect.left +rect.width  && foes[ i ]->getScaleX() > 0 )
						{
							foes[ i ]->moveX( -elapsedTime );
						}
						else if( foes[ i ]->ableToAttack() )
						{
							foes[ i ]->setAttack();
						}
						else
						{
							foes[ i ]->setIdle();
						}
					}
				}
			}
		}
	}
}

template <typename F>
float Factory <F>::getDamage()
{
	return foes[ damaged ]->getDamage();
}

template class Factory <Skeleton>;
template class Factory <Zombie>;