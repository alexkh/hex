/*------------------------------------------------------------------------------
Hex game class implementation
------------------------------------------------------------------------------*/
#include <chrono>
#include <iostream>
#include <utility> // pair

#include "hexboard.hpp"
#include "hexgame.hpp"
#include "moveeval.hpp"

using namespace std;

void HexGame::start_prompt()
{
    cout << "+---------------------------+" << endl;
    cout << "|        Hex game           |" << endl;
    cout << "| End the game with ctrl+c. |" << endl;
    cout << "+---------------------------+" << endl;
    cout << endl;
}

void HexGame::player_setup_prompt_and_set()
{
    cout << "=========================" << endl;
    cout << "Setup:" << endl;

    bool entry_ok;
    do {
        cout << "Player X (plays first): (A)I or (H)uman?: ";
        string type;
        cin >> type;
        // This is not very clean, but suffice for now.
        entry_ok = char_to_player_type_set(type[0], player_X_type);
    } while (!entry_ok);

    do {
        cout << "Player O: (A)I or (H)uman?: ";
        string type;
        cin >> type;
        entry_ok = char_to_player_type_set(type[0], player_O_type);
    } while (!entry_ok);
}

bool HexGame::next_prompt_and_play()
{
    cout << "=========================" << endl;
    cout << board << endl << endl;
    cout << "X plays north and south." << endl;
    cout << "O plays west and east." << endl;
    cout << endl;

    pair<unsigned, unsigned> move;
    bool valid_move;

    do {
        cout << "Player " << current_player << ", please enter your move: ";

        if (current_player_type_get() == PlayerType::HUMAN) {
            valid_move = human_input_get(move);
        } else if (current_player_type_get() == PlayerType::AI) {
            valid_move = ai_input_get(move);
        }
    } while (!valid_move);

    bool win = board.play(move, current_player);

    if (win) {
        winner = current_player;
    } else {
        current_player.swap();
    }

    return win;
}

void HexGame::winner_print()
{
    cout << endl;
    cout << board << endl << endl;
    cout << "\t\t!!! Player " << winner << " wins !!!" << endl;
}

PlayerType HexGame::current_player_type_get()
{
    if (current_player.get() == player_e::X) {
        return player_X_type;
    } else if (current_player.get() == player_e::O) {
        return player_O_type;
    } else {
        cerr << __func__ << ": error in player type." << endl;
        exit(1);
    }
}

// Get the input pair in 0 based integers, return false if there was an error.
bool HexGame::human_input_get(pair<unsigned, unsigned>& move)
{
    string in;
    cin >> in;
    unsigned col = toupper(in[0]) - 'A';

    // Make the first char of the string a leading 0 (as suggested in the
    // forums).
    in[0] = '0';
    unsigned row = atoi(in.c_str()) - 1;

    if ((col >= board.size_get()) || (row >= board.size_get()) ||
        board.occupied_check(col, row)
        ) {
        cout << "############ Unauthorized move";
        move_print(make_pair(col, row));
        cout << ", try again! ###########" << endl;
        return false;
    }

    move.first = col;
    move.second = row;
    return true;
}

bool HexGame::ai_input_get(pair<unsigned, unsigned>& move)
{
    cout << "thinking... " << flush;

    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();

    MoveEvaluator evaluator(board, current_player, 10000000, 5000u);
    move = evaluator.best_move_calculate();
    cout << " -> ";
    move_print(move);
    cout << endl;

    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    cout << "Elapsed time: " << elapsed_seconds.count() << " s" << endl;

    return true;    // The AI only gives valid moves.
}

bool HexGame::char_to_player_type_set(const char c, PlayerType& type)
{
    if (toupper(c) == 'H') {
        type = PlayerType::HUMAN;
        return true;
    } else if (toupper(c) == 'A') {
        type = PlayerType::AI;
        return true;
    } else {
        return false;
    }
}

int HexGame::autoplay(char color, unsigned short board_side, size_t iter) {
	board.iter = iter;
	char column; // letter representing board column from a-z
	unsigned short col; // numeric column
	unsigned short row; // numeric row
	pair<unsigned, unsigned> move;
	char c; // used for input processing
	bool game_over = false; // each player's responsibility to check winner
	// send handshake message color: name of program by author
	// this string should uniquely identify the player
	cout << color << ": hex v0.1 by Gauthier Östervall adapted by AK "
			"https://github.com/fleutot/hex\n" << flush;
	if(color == 'X') {
		// set the player types
		player_X_type = PlayerType::AI;
		player_O_type = PlayerType::HUMAN;
		// wait for other player's handshake message
		cin >> c; // should be the other player's color
		if(c != 'O') {
			cout << "X. E: expecting handshake message from O\n"
				<< flush;
			return -2;
		}
		cin >> c; // should be ':'
		if(c != ':') {
			cout << "X. E: expecting : after O in "
				"handshake message\n" << flush;
			return -3;
		}
		// ignore the rest of the line
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		// start the timer
		auto start = std::chrono::steady_clock::now();
		// make move
		{
		    MoveEvaluator evaluator(board, current_player,
				10000000, 5000u);
		    move = evaluator.best_move_calculate();
		}
		board.play(move, current_player);
		current_player.swap();
		// stop the timer
		auto end = std::chrono::steady_clock::now();
		int tmilli = std::chrono::duration<double, std::milli>
			(end - start).count();
		cout << color << char(move.first + 'a') << (move.second + 1)
			<< " #1 t=" << tmilli << "ms\n" << flush;
	} else {
		// set the player types
		player_X_type = PlayerType::HUMAN;
		player_O_type = PlayerType::AI;
	}
	int counter = 1; // count the moves
	while(true) {
		cin >> c; // other player color
		cin >> column; // lower case letter represenging board column
		if(c != (color=='O'? 'X': 'O') || column == ':') {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}
		if(column == '.') { // the other player quits, game over
			break;
		}
		col = column - 'a';
		if(col >= board_side) {
			cout << color <<  ". E: " << color <<
			" received illegal column: '" << c << "'\n";
			return -4;
		}
		cin >> row;
		if(row > board_side) {
			cout << color << ". E: " << color <<
			" received illegal row: '" << row << "'\n";
			return -5;
		}
		c = cin.peek();
		if(c == '.') { // dot at the end of the other player's move
			// means that he wins, or maybe he gives up - game over
			break;
		}
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		// start the timer
		auto start = std::chrono::steady_clock::now();
		if(color == 'X') {
			++counter;
		}
		row--; // convert to 0-based representation
		// register opponent's move
		if(board.occupied_check(col, row)) {
			cout << color << ". E: " << " received illegal "
				<< "move " << column << row << "\n";
			return -6;
		}
		move.first = col;
		move.second = row;
		game_over = board.play(move, current_player);
		// check if game is over
		if(game_over) {
			break;
		}
		current_player.swap();
		// make a move. If I won, add a dot.
		{
		    MoveEvaluator evaluator(board, current_player,
				10000000, 5000u);
		    move = evaluator.best_move_calculate();
		}
		// make move and check if game is over
		game_over = board.play(move, current_player);
		current_player.swap();
		// stop the timer
		auto end = std::chrono::steady_clock::now();
		int tmilli = std::chrono::duration<double, std::milli>
			(end - start).count();
		cout << color << char(move.first + 'a') << (move.second + 1)
			<< (game_over? '.': ' ') << '#' << counter
			<< " t=" << tmilli << "ms\n" << flush;
		if(game_over) {
			break;
		}
		if(color == 'O') {
			++counter;
		}
	}
	return 0;
}
