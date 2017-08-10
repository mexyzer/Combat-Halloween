#include "play.h"
#include "own/file.h"

Play::Play()
{
	free();
}

Play::~Play()
{
	free();
}

void Play::free()
{
	level = false;
	menu = false;
	table = false;
	run = false;
	
	background.free();
	homebutton.free();
	levelbutton.free();
	chunkbutton.free();
	musicbutton.free();
	chunk_volume.free();
	music_volume.free();
	pausesystem.free();
	music.free();
}

void Play::load( float screen_w, float screen_h )
{
	free();
	
	float scale_x = screen_w /2560;
	float scale_y = screen_h /1440;
	
	// Load background.
	background.setIdentity( "play-background" );
	background.load( "images/play/background.png" );
	background.setScale( scale_x, scale_y );
	
	// Circle buttons.
	homebutton.load( "images/level/home.png" );
	homebutton.setPosition( screen_w /256 , screen_h /144 +homebutton.getHeight(), scale_x, scale_y );
	homebutton.setPosition( screen_w /256 +homebutton.getWidth(), screen_h /144 +homebutton.getHeight(), scale_x, scale_y );
	levelbutton.load( "images/play/level.png" );
	levelbutton.setPosition( homebutton.getRight() +screen_w /256 +homebutton.getWidth(), screen_h /144 +homebutton.getHeight(), scale_x, scale_y );
	chunkbutton.load( "images/menu/chunk.png", true );
	chunkbutton.setPosition( screen_w -screen_w /256 , screen_h -screen_h /144, scale_x, scale_y );
	musicbutton.load( "images/menu/music.png", true );
	musicbutton.setPosition( chunkbutton.getLeft() -screen_w /256, screen_h -screen_h /144, scale_x, scale_y );
	
	// Set volume buttons.
	chunk_volume.load( chunkbutton.getLeft(), chunkbutton.getRight(), chunkbutton.getBot(), screen_w, screen_h );
	music_volume.load( musicbutton.getLeft(), musicbutton.getRight(), musicbutton.getBot(), screen_w, screen_h );
	
	// Pause system.
	pausesystem.load( screen_w, screen_h );
	
	// Music.
	music.setIdentity( "play-music" );
	music.load( "music/play.ogg" );
}

void Play::handle( sf::Event& event )
{
	if( !menu && !level && !table )
	{
		if( !pausesystem.isActive() && pausesystem.getAlpha() == 0 )
		{
			homebutton.handle( event );
			levelbutton.handle( event );
			
			if( !chunk_volume.handle( event ) )
			{
				chunkbutton.handle( event );
			}
			
			if( !music_volume.handle( event ) )
			{
				musicbutton.handle( event );
			}
		}
		
		pausesystem.handle( event );
	}
}

void Play::head( sf::RenderWindow* &window, double elapsedTime )
{
	mechanics( elapsedTime );
	fades( elapsedTime );
	draw( window );
}



void Play::draw( sf::RenderWindow* &window )
{
	window->draw( background.get() );
	homebutton.draw( window );
	levelbutton.draw( window );
	chunkbutton.draw( window );
	musicbutton.draw( window );
	chunk_volume.draw( window );
	music_volume.draw( window );
	pausesystem.draw( window );
}

void Play::mechanics( double elapsedTime )
{
	// Mechanics.
	if( !pausesystem.isActive() && !menu && !level && !table )
	{
		// Someone clicked home button.
		if( homebutton.isChanged() )
		{
			menu = true;
		}
		
		// Someone clicked play button.
		else if( levelbutton.isChanged() )
		{
			level = true;
		}
		
		chunk_volume.mechanics( elapsedTime );
		music_volume.mechanics( elapsedTime );
		
		// Turn on/off all sounds.
		if( chunkbutton.isChanged() )
		{
			homebutton.setPlayable( chunkbutton.isActive() );
			levelbutton.setPlayable( chunkbutton.isActive() );
			chunkbutton.setPlayable( !chunkbutton.isActive() );
			musicbutton.setPlayable( chunkbutton.isActive() );
			chunk_volume.setPlayable( chunkbutton.isActive() );
			music_volume.setPlayable( chunkbutton.isActive() );
			chunk_volume.setActive( chunkbutton.isActive() );
			pausesystem.setPlayable( chunkbutton.isActive() );
		}
		
		// Volume of sounds is changed.
		if( chunk_volume.isChanged() )
		{
			float value = chunk_volume.getMainVolume();
			homebutton.setVolume( value );
			levelbutton.setVolume( value );
			chunkbutton.setVolume( value );
			musicbutton.setVolume( value );
			chunk_volume.setVolume( value );
			music_volume.setVolume( value );
			pausesystem.setVolume( value );
		}
		
		// Turn on/off music.
		if( musicbutton.isChanged() )
		{
			musicbutton.isActive() ? music.play() : music.pause();
			music_volume.setActive( musicbutton.isActive() );
		}
		
		// Volume of music is changed.
		if( music_volume.isChanged() )
		{
			music.setVolume( music_volume.getMainVolume() );
		}
	}
}

void Play::fades( double elapsedTime )
{
	// Fade out - paused.
	if( pausesystem.isActive() )
	{
		float value = elapsedTime *0xFF;
		float min = 0xFF *3 /4;
		
		background.fadeout( value, min );
		homebutton.fadeout( value, min );
		levelbutton.fadeout( value, min );
		chunkbutton.fadeout( value, min );
		musicbutton.fadeout( value, min );
		chunk_volume.fadeout( value, min );
		music_volume.fadeout( value, min );
		pausesystem.fadein( value *3, min );
		
		music.fadeout( elapsedTime *100, music_volume.getMainVolume() *0.2 );
	}
	
	// Fade out - closed.
	else if( menu || level || table )
	{
		float value = elapsedTime *0xFF /2;
		
		background.fadeout( value );
		homebutton.fadeout( value );
		levelbutton.fadeout( value );
		chunkbutton.fadeout( value );
		musicbutton.fadeout( value );
		chunk_volume.fadeout( value );
		music_volume.fadeout( value );
		
		music.fadeout( elapsedTime *100 );
	}
	
	// Fade in.
	else
	{
		float value = elapsedTime *0xFF;
		
		background.fadein( value );
		homebutton.fadein( value );
		levelbutton.fadein( value );
		chunkbutton.fadein( value );
		musicbutton.fadein( value );
		chunk_volume.fadein( value );
		music_volume.fadein( value );
		pausesystem.fadeout( value );
		
		music.fadein( elapsedTime *100, music_volume.getMainVolume() );
	}
}


	
void Play::loadSound()
{
	if( !run )
	{
		run = true;
		
		// Set thread.
		// knight_specs.setThread();
		// information.setThread();
		
		float chunkVolume = 0;
		float musicVolume = 0;
		bool chunkPlay = false;
		bool musicPlay = false;
		
		MyFile file;
		file.load( "txt/sound.txt" );
		if( file.is_good() )
		{
			string line = "";
			file.get() >> line; chunkVolume = con::stof( line );
			file.get() >> line; musicVolume = con::stof( line );
			file.get() >> line; chunkPlay = con::stoi( line );
			file.get() >> line; musicPlay = con::stoi( line );
			
			// The rest.
			// printf( "%f %f %d %d\n", chunkVolume, musicVolume, chunkPlay, musicPlay );
			
			// Chunk.
			homebutton.setVolume( chunkVolume );
			levelbutton.setVolume( chunkVolume );
			chunkbutton.setVolume( chunkVolume );
			musicbutton.setVolume( chunkVolume );
			chunk_volume.setVolume( chunkVolume );
			chunk_volume.setMainVolume( chunkVolume );
			music_volume.setVolume( chunkVolume );
			pausesystem.setVolume( chunkVolume );
			
			// Music.
			music.setVolume( musicVolume );
			music_volume.setMainVolume( musicVolume );
			
			// Set playable.
			homebutton.setPlayable( chunkPlay );
			levelbutton.setPlayable( chunkPlay );
			chunkbutton.setPlayable( chunkPlay );
			chunkbutton.setActive( chunkPlay );
			musicbutton.setPlayable( chunkPlay );
			musicbutton.setActive( musicPlay );
			chunk_volume.setPlayable( chunkPlay );
			music_volume.setPlayable( chunkPlay );
			pausesystem.setPlayable( chunkPlay );
			musicPlay ? music.play() : music.pause();
		}
		
		file.free();
	}
}

void Play::saveSound()
{
	MyFile file;
	file.load( "txt/sound.txt" );
	if( file.is_good() )
	{
		string line = con::itos( chunk_volume.getMainVolume() ); file.get() << line << endl;
		line = con::itos( music_volume.getMainVolume() ); file.get() << line << endl;
		line = con::itos( chunkbutton.isActive() ); file.get() << line << endl;
		line = con::itos( musicbutton.isActive() ); file.get() << line;
	}
	
	file.free();
	
	// plus
	menu = false;
	level = false;
	table = false;
	run = false;
	
	// reset buttons
	homebutton.setActive( false );
	levelbutton.setActive( false );
}



bool Play::isMenu()
{
	if( menu && background.getAlpha() == 0 )
	{
		return true;
	}
	
	return false;
}

bool Play::isLevel()
{
	if( level && background.getAlpha() == 0 )
	{
		return true;
	}
	
	return false;
}

bool Play::isTable()
{
	if( table && background.getAlpha() == 0 )
	{
		return true;
	}
	
	return false;
}