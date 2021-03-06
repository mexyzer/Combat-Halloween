#include "menu.h"
#include "own/file.h"

Menu::Menu()
{
	free();
}

Menu::~Menu()
{
	free();
}

void Menu::free()
{
	ready = false;
	close = false;
	editor = false;
	run = false;
	
	background.free();
	knight_specs.free();
	github.free();
	scores.free();
	website.free();
	singleplayer.free();
	multiplayer.free();
	exit.free();
	chunkbutton.free();
	musicbutton.free();
	updatebutton.free();
	reloadbutton.free();
	settingsbutton.free();
	settings.free();
	chunk_volume.free();
	music_volume.free();
	information.free();
	chat.free();
	pausesystem.free();
	music.free();
}



void Menu::load( float screen_w, float screen_h )
{
	free();
	
	float scale_x = screen_w /2560;
	float scale_y = screen_h /1440;
	
	// Load background.
	background.setIdentity( "menu-background" );
	background.load( "images/menu/background.png" );
	background.setScale( scale_x, scale_y );
	
	// Load "knight specs".
	knight_specs.load( screen_w, screen_h );
	
	// Load link buttons.
	github.load( screen_w /256, screen_h /144, "images/menu/github.png" );
	github.setScale( scale_x, scale_y );
	github.setUrl( "https://github.com/Adriqun/Combat-Halloween" );
	scores.load( github.getRight() +screen_w /256, screen_h /144, "images/menu/scores.png" );
	scores.setScale( scale_x, scale_y );
	scores.setUrl( "scores" );
	website.load( scores.getRight() +screen_w /256, screen_h /144, "images/menu/website.png" );
	website.setScale( scale_x, scale_y );
	website.setUrl( "http://adrianmichalek.pl/combathalloween/" );
	
	// Main buttons.
	singleplayer.load( screen_w /1.9, screen_h /2.35, "images/menu/singleplayer.png" );
	singleplayer.setScale( scale_x, scale_y );
	multiplayer.load( screen_w /1.9, singleplayer.getBot() +screen_h /72, "images/menu/multiplayer.png", true );
	multiplayer.setScale( scale_x, scale_y );
	exit.load( screen_w /1.9, multiplayer.getBot() +screen_h /72, "images/menu/exit.png" );
	exit.setScale( scale_x, scale_y );
	
	// Circle buttons.
	chunkbutton.load( "images/menu/chunk.png" );
	chunkbutton.setPosition( screen_w -screen_w /256, screen_h -screen_h /144, scale_x, scale_y );
	musicbutton.load( "images/menu/music.png" );
	musicbutton.setPosition( chunkbutton.getLeft() -screen_w /256, screen_h -screen_h /144, scale_x, scale_y );
	updatebutton.load( "images/level/update.png" );
	updatebutton.setPosition( website.getRight() +chunkbutton.getWidth() +screen_w /256, screen_h /144 +chunkbutton.getHeight(), scale_x, scale_y );
	reloadbutton.load( "images/menu/reload.png" );
	reloadbutton.setPosition( singleplayer.getLeft() -screen_w /256, singleplayer.getTop() +musicbutton.getHeight() +screen_h /72, scale_x, scale_y );
	settingsbutton.load( "images/menu/settings.png" );
	settingsbutton.setPosition( musicbutton.getLeft() -screen_w /256, screen_h -screen_h /144, scale_x, scale_y );
	
	// Settings.
	settings.load( screen_w, screen_h );
	
	// Set volume buttons.
	chunk_volume.load( chunkbutton.getLeft(), chunkbutton.getRight(), chunkbutton.getBot(), screen_w, screen_h );
	music_volume.load( musicbutton.getLeft(), musicbutton.getRight(), musicbutton.getBot(), screen_w, screen_h );
	
	// Load information.
	information.load( screen_w, screen_h );
	
	// Set chat.
	chat.load( screen_w, screen_h );
	chat.setCommandColor( sf::Color( 0, 0, 0 ) );
	chat.setTypicalColor( sf::Color( 0x68, 0x68, 0x68 ) );
	
	// Pause system.
	pausesystem.load( screen_w, screen_h );
	
	// Music.
	music.setIdentity( "menu-music" );
	music.load( "music/menu.ogg" );
}

void Menu::handle( sf::Event& event )
{
	if( !close && !ready && !editor )
	{
		if( !pausesystem.isActive() && pausesystem.getAlpha() == 0 )
		{
			chat.handle( event );
			
			if( !chat.isOpen() )
			{
				knight_specs.handle( event );
				github.handle( event );
				scores.handle( event );
				website.handle( event );
				singleplayer.handle( event );
				multiplayer.handle( event );
				exit.handle( event );
				
				if( !chunk_volume.handle( event ) )
				{
					chunkbutton.handle( event );
				}
				
				if( !music_volume.handle( event ) )
				{
					musicbutton.handle( event );
				}
				
				if( !knight_specs.isReady() || !information.isReady() )
				{
					reloadbutton.handle( event );
				}
				else
				{
					updatebutton.handle( event );
				}
				
				settingsbutton.handle( event );
				settings.handle( event );
			}
		}
		
		if( !chat.isOpen() )
		{
			pausesystem.handle( event );
		}
	}
}

void Menu::head( sf::RenderWindow* &window, double elapsedTime )
{
	mechanics( elapsedTime );
	fades( elapsedTime );
	draw( window );
}



void Menu::draw( sf::RenderWindow* &window )
{
	window->draw( background.get() );
	knight_specs.draw( window );
	github.draw( window );
	scores.draw( window );
	website.draw( window );
	singleplayer.draw( window );
	multiplayer.draw( window );
	exit.draw( window );
	chunkbutton.draw( window );
	musicbutton.draw( window );
	
	if( !knight_specs.isReady() || !information.isReady() )
	{
		reloadbutton.draw( window );
	}
	else
	{
		updatebutton.draw( window );
	}
	
	settingsbutton.draw( window );
	settings.draw( window );
	chunk_volume.draw( window );
	music_volume.draw( window );
	information.draw( window );
	chat.draw( window );
	pausesystem.draw( window );
}

void Menu::mechanics( double elapsedTime )
{
	// Mechanics.
	if( !pausesystem.isActive() && !close && !ready && !editor )
	{
		chat.mechanics( elapsedTime );
		if( chat.isCommand() )
		{
			// Knight specs.
			if( chat.findCommand( "@clear" ) )			knight_specs.setChosen( -1 );
			else if( chat.findCommand( "@helmet" ) )	knight_specs.setChosen( 0 );
			else if( chat.findCommand( "@body" ) )		knight_specs.setChosen( 1 );
			else if( chat.findCommand( "@shield" ) )	knight_specs.setChosen( 2 );
			else if( chat.findCommand( "@sword" ) )		knight_specs.setChosen( 3 );
			else if( chat.findCommand( "@boots" ) )		knight_specs.setChosen( 4 );
			
			// Close application.
			else if( chat.findCommand( "@close" ) || chat.findCommand( "@exit" ) )
			{
				exit.setPressed();
			}
			
			// Someone clicked singleplayer.
			else if( chat.findCommand( "@start" ) || chat.findCommand( "@play" ) )
			{
				if( knight_specs.isReady() && information.isReady() )
				{
					singleplayer.setPressed();
				}
			}
			
			// Exsert / shovel settings.
			else if( chat.findCommand( "@settings" ) || chat.findCommand( "@keyboard" ) ||
			chat.findCommand( "@keys" ) || chat.findCommand( "@sets" ) )
			{
				settingsbutton.setActive( !settingsbutton.isActive() );
			}
			
			// Reload data.
			else if( chat.findCommand( "@reload" ) || chat.findCommand( "@connect" ) ||
			chat.findCommand( "@rel" ) || chat.findCommand( "@con" ) )
			{
				reloadbutton.setActive( true );
			}
			
			// Update data.
			else if( chat.findCommand( "@update" ) )
			{
				updatebutton.setActive( true );
			}
			
			// Turn on/off all chunks.
			else if( chat.findCommand( "@chunk" ) )
			{
				chunkbutton.setChanged( true );
				chunkbutton.setActive( !chunkbutton.isActive() );
			}
			
			// Turn on/off music.
			else if( chat.findCommand( "@music" ) )
			{
				musicbutton.setChanged( true );
				musicbutton.setActive( !musicbutton.isActive() );
			}
			
			// Turn on/off all sounds.
			else if( chat.findCommand( "@sound" ) )
			{
				chunkbutton.setChanged( true );
				chunkbutton.setActive( !chunkbutton.isActive() );
				musicbutton.setChanged( true );
				musicbutton.setActive( !musicbutton.isActive() );
			}
			
			// Map editor.
			else if( chat.findCommand( "@editor" ) || chat.findCommand( "@edit" ) )
			{
				editor = true;
				chat.isOpen() = false;
			}
			
			// Link buttons in addition.
			else if( chat.findCommand( "@github" ) )		github.openWebsite();
			else if( chat.findCommand( "@scores" ) )		scores.openWebsite();
			else if( chat.findCommand( "@website" ) )	website.openWebsite();
			
			// Tell that command doesn't exist.
			else
			{
				chat.setError();
			}
		}
		
		// Knight specs
		knight_specs.mechanics( elapsedTime );
		
		
		// Close application.
		if( exit.isPressed() )
		{
			chat.isOpen() = false;
			close = true;
		}
		
		
		// Someone clicked singleplayer.
		if( singleplayer.isPressed() )
		{
			chat.isOpen() = false;
			ready = true;
		}
		
		
		// Exsert / shovel settings.
		if( settingsbutton.isActive() )
		{
			settings.exsertTable( elapsedTime );
		}
		else
		{
			settings.shovelTable( elapsedTime );
		}
		
		// if we dont have answer from database
		if( !knight_specs.isReady() || !information.isReady() )
		{
			singleplayer.lock();
		}
		else
		{
			singleplayer.unlock();
		}
		
		// update data
		if( updatebutton.isActive() )
		{
			updatebutton.setActive( false );
			
			if( knight_specs.isReady() && information.isReady() )
			{
				knight_specs.reloadValues();
				knight_specs.setThread();
				information.reloadMoney();
				information.setThread();
			}
		}
		
		// reload data
		if( reloadbutton.isActive() )
		{
			reloadbutton.setActive( false );
			if( !knight_specs.isReady() )
			{
				knight_specs.setThread();
			}
			
			if( !information.isReady() )
			{
				information.setThread();
			}
		}
		
		settings.mechanics( elapsedTime );
		chunk_volume.mechanics( elapsedTime );
		music_volume.mechanics( elapsedTime );
		
		// Turn on/off all chunks.
		if( chunkbutton.isChanged() )
		{
			github.setPlayable( chunkbutton.isActive() );
			scores.setPlayable( chunkbutton.isActive() );
			website.setPlayable( chunkbutton.isActive() );
			singleplayer.setPlayable( chunkbutton.isActive() );
			multiplayer.setPlayable( chunkbutton.isActive() );
			exit.setPlayable( chunkbutton.isActive() );
			chunkbutton.setPlayable( !chunkbutton.isActive() );
			musicbutton.setPlayable( chunkbutton.isActive() );
			settingsbutton.setPlayable( chunkbutton.isActive() );
			settings.setPlayable( chunkbutton.isActive() );
			chunk_volume.setPlayable( chunkbutton.isActive() );
			chunk_volume.setActive( chunkbutton.isActive() );
			music_volume.setPlayable( chunkbutton.isActive() );
			pausesystem.setPlayable( chunkbutton.isActive() );
		}
		
		// Volume of sounds is changed.
		if( chunk_volume.isChanged() )
		{
			float value = chunk_volume.getMainVolume();
			knight_specs.setVolume( value );
			github.setVolume( value );
			scores.setVolume( value );
			website.setVolume( value );
			singleplayer.setVolume( value );
			multiplayer.setVolume( value );
			exit.setVolume( value );
			chunkbutton.setVolume( value );
			musicbutton.setVolume( value );
			settingsbutton.setVolume( value );
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

void Menu::fades( double elapsedTime )
{
	// Fade out - paused.
	if( pausesystem.isActive() )
	{
		float value = elapsedTime *0xFF *2;
		float min = 0xFF *3 /4;
		
		background.fadeout( value, min );
		knight_specs.fadeout( value, min );
		github.fadeout( value, min );
		scores.fadeout( value, min );
		website.fadeout( value, min );
		singleplayer.fadeout( value, min );
		multiplayer.fadeout( value, min );
		exit.fadeout( value, min );
		chunkbutton.fadeout( value, min );
		musicbutton.fadeout( value, min );
		updatebutton.fadeout( value, min );
		reloadbutton.fadeout( value, min );
		settingsbutton.fadeout( value, min );
		settings.fadeout( value, min );
		chunk_volume.fadeout( value, min );
		music_volume.fadeout( value, min );
		information.fadeout( value, min );
		chat.fadeout( value, min );
		pausesystem.fadein( value *3, min );
		
		music.fadeout( elapsedTime *100, music_volume.getMainVolume() *0.2 );
	}
	
	// Fade out - closed.
	else if( close || ready || editor )
	{
		float value = elapsedTime *0xFF;
		
		background.fadeout( value );
		knight_specs.fadeout( value );
		github.fadeout( value );
		scores.fadeout( value );
		website.fadeout( value );
		singleplayer.fadeout( value );
		multiplayer.fadeout( value );
		exit.fadeout( value );
		chunkbutton.fadeout( value );
		musicbutton.fadeout( value );
		updatebutton.fadeout( value );
		reloadbutton.fadeout( value );
		settingsbutton.fadeout( value );
		settings.fadeout( value );
		chunk_volume.fadeout( value );
		music_volume.fadeout( value );
		information.fadeout( value );
		chat.fadeout( value );
		
		music.fadeout( elapsedTime *100 );
	}
	
	// Fade in.
	else
	{
		float value = elapsedTime *0xFF *2;
		
		background.fadein( value );
		knight_specs.fadein( value );
		github.fadein( value );
		scores.fadein( value );
		website.fadein( value );
		singleplayer.fadein( value );
		multiplayer.fadein( value );
		exit.fadein( value );
		chunkbutton.fadein( value );
		musicbutton.fadein( value );
		updatebutton.fadein( value );
		reloadbutton.fadein( value );
		settingsbutton.fadein( value );
		settings.fadein( value );
		chunk_volume.fadein( value );
		music_volume.fadein( value );
		information.fadein( value );
		pausesystem.fadeout( value );
		
		music.fadein( elapsedTime *100, music_volume.getMainVolume() );
	}
}



void Menu::setUsername( string line )
{
	knight_specs.setUsername( line );
	information.setUsername( line );
	chat.setUsername( line );
}

void Menu::loadSound()
{
	if( !run )
	{
		run = true;
		
		// Set thread.
		knight_specs.setThread();
		information.setThread();
		
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
			knight_specs.setVolume( chunkVolume );
			github.setVolume( chunkVolume );
			scores.setVolume( chunkVolume );
			website.setVolume( chunkVolume );
			singleplayer.setVolume( chunkVolume );
			multiplayer.setVolume( chunkVolume );
			exit.setVolume( chunkVolume );
			chunkbutton.setVolume( chunkVolume );
			musicbutton.setVolume( chunkVolume );
			updatebutton.setVolume( chunkVolume );
			reloadbutton.setVolume( chunkVolume );
			settingsbutton.setVolume( chunkVolume );
			settings.setVolume( chunkVolume );
			chunk_volume.setVolume( chunkVolume );
			chunk_volume.setMainVolume( chunkVolume );
			music_volume.setVolume( chunkVolume );
			pausesystem.setVolume( chunkVolume );
			
			// Music.
			music.setVolume( musicVolume );
			music_volume.setMainVolume( musicVolume );
			
			// Set playable.
			knight_specs.setPlayable( chunkPlay );
			github.setPlayable( chunkPlay );
			scores.setPlayable( chunkPlay );
			website.setPlayable( chunkPlay );
			singleplayer.setPlayable( chunkPlay );
			multiplayer.setPlayable( chunkPlay );
			exit.setPlayable( chunkPlay );
			chunkbutton.setPlayable( !chunkPlay );
			chunkbutton.setActive( chunkPlay );
			musicbutton.setPlayable( chunkPlay );
			musicbutton.setActive( musicPlay );
			updatebutton.setPlayable( chunkPlay );
			reloadbutton.setPlayable( chunkPlay );
			settingsbutton.setPlayable( chunkPlay );
			settings.setPlayable( chunkPlay );
			chunk_volume.setPlayable( chunkPlay );
			music_volume.setPlayable( chunkPlay );
			pausesystem.setPlayable( chunkPlay );
			musicPlay ? music.play() : music.pause();
		}
		
		file.free();
	}
}

void Menu::saveSound()
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
	ready = false;
	close = false;
	editor = false;
	run = false;
	
	// Reset.
	knight_specs.reload();
	singleplayer.reload();
	settingsbutton.setActive( false );
	settings.reload();
	chat.reset();
	music.stop();
}



bool Menu::isReady()
{
	if( ready && background.getAlpha() == 0 )
	{
		return true;
	}
	
	return false;
}

bool Menu::isClose()
{
	if( close && background.getAlpha() == 0 )
	{
		return true;
	}
	
	return false;
}

bool Menu::isEditor()
{
	if( editor && background.getAlpha() == 0 )
	{
		return true;
	}
	
	return false;
}