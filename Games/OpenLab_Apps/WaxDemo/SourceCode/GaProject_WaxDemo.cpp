////////////////////////////////////////////////////////////////////////////////
// GaProject_WaxDemo.cpp

// Includes
#include "SbMain.h"
#include "game.h"

SbMain project;

////////////////////////////////////////////////////////////////////////////////
// main

int main()
{
	Game game(&project);

	// Create the game
	game.Create();

	// Exit the application
	exit(EXIT_SUCCESS);
}
