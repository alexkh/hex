/*------------------------------------------------------------------------------
hex.cpp
Run a game of hex.
------------------------------------------------------------------------------*/
#include <cstdlib>  // atoi, rand
#include <ctime>
#include <iostream>
#include <utility>  // pair
#include <sstream>

#include "hexgame.hpp"
#include "moveeval.hpp"

using namespace std;

// autoplay: <program name> (X|O) [<board side>] [<iterations>]
// example: hex X 11 1000
// interactive play: <program name> <board side>
// example: hex 11
int main(int argc, char *argv[])
{
	char color = 'X'; // can be X or O
	unsigned short board_side = 11; // side of the board minimum 3
	size_t iter = 1000; // number of iterations should be selectable
	// parse command line parameters
	argc = argc > 4? 4: argc; // forward compatibility measure
	switch(argc) {
	case 4:
	{
		stringstream ss; // used for reading numbers from strings
		ss << argv[3]; // put third argument into the stringstream
		ss >> iter; // read its numeric value
	}
	case 3:
	{
		stringstream ss;
		ss << argv[2];
		ss >> board_side;
		board_side = board_side < 3? 3: board_side; // minimum 3
	}
	case 2:
		color = argv[1][0];
		if(color != 'X' && color != 'O') {
			break; // try interactive play
		}
		{
			HexGame game(board_side);
			game.autoplay(color, board_side, iter);
			return 0;
		}
	case 1: ; // no command line arguments - continue with interactive play
	}

    const int max_width = 16u;

    if ((argc != 2) || (atoi(argv[1]) > max_width))  {
        cout << "Usage: hex n" << endl;
        cout << "    n: the width of the board, less than " << max_width
             << endl;
        exit(1);
    }

    HexGame game(atoi(argv[1]));

    game.start_prompt();
    game.player_setup_prompt_and_set();

    bool win;

    do {
        win = game.next_prompt_and_play();
    } while (!win);

    game.winner_print();
}


