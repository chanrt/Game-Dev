#define U1 box[0]
#define U2 box[1]
#define U3 box[2]
#define M1 box[3]
#define M2 box[4]
#define M3 box[5]
#define D1 box[6]
#define D2 box[7]
#define D3 box[8]

#define MOUSE_X sf::Mouse::getPosition(window).x
#define MOUSE_Y sf::Mouse::getPosition(window).y

#include<SFML/Audio.hpp>
#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Window.hpp>
#include <iostream>
#include <string>
#include "wtypes.h"

int box[9];
bool ai_play;
int self; // during ai_play, self contains symbol of computer. 1 corresponds to X and 2 corresponds to O
int player_x; // during two player, p1 contains index of player with X as symbol
std::string player1, player2; // during ai_play, player1 becomes name of player
int chance; // denotes the current chance. 1 refers to player 1/player and 2 refers to player 2/computer
bool play_again;

void init();
void delay(float n_seconds);

int ai()
{
	int opp;
	if (self == 2) opp = 1; else opp = 2;

	// Victory conditions
	if (U2 == self && U3 == self || M1 == self && D1 == self || M2 == self && D3 == self) if (U1 == 0) return 1;
	if (U1 == self && U3 == self || M2 == self && D2 == self) if (U2 == 0) return 2;
	if (U1 == self && U2 == self || M3 == self && D3 == self || M2 == self && D1 == self) if (U3 == 0) return 3;
	if (U1 == self && D1 == self || M2 == self && M3 == self) if (M1 == 0) return 4;
	if (M1 == self && M3 == self || U2 == self && D2 == self || U1 == self && D3 == self || U3 == self && D1 == self) if (M2 == 0) return 5;
	if (M1 == self && M2 == self || U3 == self && D3 == self) if (M3 == 0) return 6;
	if (U1 == self && M1 == self || D2 == self && D3 == self || U3 == self && M2 == self) if (D1 == 0) return 7;
	if (U2 == self && M2 == self || D1 == self && D3 == self) if (D2 == 0) return 8;
	if (U3 == self && M3 == self || D1 == self && D2 == self || U1 == self && M2 == self) if (D3 == 0) return 9;

	// Prevent defeat conditions
	if (U2 == opp && U3 == opp || M1 == opp && D1 == opp || M2 == opp && D3 == opp) if (U1 == 0) return 1;
	if (U1 == opp && U3 == opp || M2 == opp && D2 == opp) if (U2 == 0) return 2;
	if (U1 == opp && U2 == opp || M3 == opp && D3 == opp || M2 == opp && D1 == opp) if (U3 == 0) return 3;
	if (U1 == opp && D1 == opp || M2 == opp && M3 == opp) if (M1 == 0) return 4;
	if (M1 == opp && M3 == opp || U2 == opp && D2 == opp || U1 == opp && D3 == opp || U3 == opp && D1 == opp) if (M2 == 0) return 5;
	if (M1 == opp && M2 == opp || U3 == opp && D3 == opp) if (M3 == 0) return 6;
	if (U1 == opp && M1 == opp || D2 == opp && D3 == opp || U3 == opp && M2 == opp) if (D1 == 0) return 7;
	if (U2 == opp && M2 == opp || D1 == opp && D3 == opp) if (D2 == 0) return 8;
	if (U3 == opp && M3 == opp || D1 == opp && D2 == opp || U1 == opp && M2 == opp) if (D3 == 0) return 9;

	// Centre domination
	if (M2 == 0) return 5;

	// Corner centered double tricks
	if (U3 == self && D1 == self && U1 == 0) return 1;
	if (U1 == self && D3 == self && U3 == 0) return 3;
	if (U1 == self && D3 == self && D1 == 0) return 7;
	if (U3 == self && D1 == self && D3 == 0) return 9;

	// Corner flanking double tricks
	if (U3 == self && D3 == self || D1 == self && D3 == self) if (U1 == 0) return 1;
	if (U1 == self && D1 == self || D1 == self && D3 == self) if (U3 == 0) return 3;
	if (U1 == self && U3 == self || U3 == self && D3 == self) if (D1 == 0) return 7;
	if (U1 == self && U3 == self || U1 == self && D1 == self) if (D3 == 0) return 9;

	// Edge centered double tricks
	if (U2 == self && M1 == self && U1 == 0) return 1;
	if (U2 == self && M3 == self && U3 == 0) return 3;
	if (M1 == self && D2 == self && D1 == 0) return 7;
	if (M3 == self && D2 == self && D3 == 0) return 9;

	// Corner tricks
	if (M1 == self && U3 == self || D1 == self && U2 == self) if (U1 == 0) return 1;
	if (U2 == self && D3 == self || U1 == self && D2 == self) if (U3 == 0) return 3;
	if (U1 == self && D2 == self || M1 == self && D3 == self) if (D1 == 0) return 7;
	if (M3 == self && D1 == self || D2 == self && U3 == self) if (D3 == 0) return 9;

	// Threats
	if (D2 == 0) if (D1 == self && D3 == 0 || D1 == 0 && D3 == self || U2 == self && M2 == 0 || U2 == 0 && M2 == self) return 8;
	if (U2 == 0) if (U1 == self && U3 == 0 || U1 == 0 && U3 == self || M2 == self && D2 == 0 || M2 == 0 && D2 == self) return 2;
	if (M3 == 0) if (U3 == self && D3 == 0 || U3 == 0 && D3 == self || M1 == self && M2 == 0 || M1 == 0 && M2 == self) return 6;
	if (M1 == 0) if (U1 == self && D1 == 0 || U1 == 0 && D1 == self || M2 == self && M3 == 0 || M2 == 0 && M3 == self) return 4;

	if (D3 == 0) if (D1 == self && D2 == 0 || D1 == 0 && D2 == self || U3 == self && M3 == 0 || U3 == 0 && M3 == self || U1 == self && M2 == 0 || U1 == 0 && M2 == self) return 9;
	if (D1 == 0) if (D2 == self && D3 == 0 || D2 == 0 && D3 == self || U1 == self && M1 == 0 || U1 == 0 && M1 == self || U3 == self && M2 == 0 || U3 == 0 && M2 == self) return 7;
	if (U1 == 0) if (U2 == self && U3 == 0 || U3 == self && U2 == 0 || D1 == self && M1 == 0 || D1 == 0 && M1 == self || M2 == self && D3 == 0 || M2 == 0 && D3 == self) return 1;	
	if (U3 == 0) if (U1 == self && U2 == 0 || U1 == 0 && U2 == self || D3 == self && M3 == 0 || D3 == 0 && M3 == self || D1 == self && M2 == 0 || M2 == self && D1 == 0) return 3;	
	
	if (M2 == 0)
	{
		if (M1 == self && M3 == 0 || M1 == 0 && M3 == self || U2 == self && D2 == 0 || U2 == 0 && D2 == self) return 5;
		if (U1 == self && D3 == 0 || U1 == 0 && D3 == self || U3 == self && D1 == 0 || U3 == 0 && D1 == self) return 5;
	}

    // No other options
	if (U1 == 0) return 1;
	if (U3 == 0) return 3;
	if (D3 == 0) return 9;
	if (D1 == 0) return 7;

	if (U2 == 0) return 2;
	if (M3 == 0) return 6;
	if (D2 == 0) return 8;
	if (M1 == 0) return 4;
}

int checkGrid()
{
	if (U1 == U2 && U2 == U3 && U3 != 0) return U3;
	if (M1 == M2 && M2 == M3 && M3 != 0) return M3;
	if (D1 == D2 && D2 == D3 && D3 != 0) return D3;
	if (U1 == M1 && M1 == D1 && D1 != 0) return D1;
	if (U2 == M2 && M2 == D2 && D2 != 0) return D2;
	if (U3 == M3 && M3 == D3 && D3 != 0) return D3;
	if (U1 == M2 && M2 == D3 && D3 != 0) return D3;
	if (U3 == M2 && M2 == D1 && D1 != 0) return D1;

	for (int i = 0; i < 9; i++)
	{
		if (box[i] == 0) return 0;
	}
	return 3;
}

int main()
{
	srand(time(NULL));
	init();
	play_again = false;

	sf::RectangleShape grid(sf::Vector2f(250,250));
	sf::Texture grid_texture;
	grid_texture.loadFromFile("xo_data/grid.png");
	grid.setTexture(&grid_texture);
	grid.setPosition(325, 100);

	sf::Font font;
	font.loadFromFile("xo_data/futura.ttf");

	sf::Text title;
	title.setFont(font);
	title.setCharacterSize(50);
	title.setStyle(sf::Text::Bold);
	title.setString("Tic Tac Toe: The Game");
	title.setPosition(60, 10);

	sf::Text symbol_x;
	symbol_x.setFont(font);
	symbol_x.setCharacterSize(30);
	symbol_x.setFillColor(sf::Color(0, 0, 200));
	symbol_x.setStyle(sf::Text::Bold);
	symbol_x.setPosition(10, 100);
	if (ai_play)
	{
		if (self == 1) symbol_x.setString("X: Computer");
		else symbol_x.setString("X:" + player1);
	}
	else
	{
		if (player_x == 1) symbol_x.setString("X: " + player1);
		else symbol_x.setString("X: " + player2);
	}

	sf::Text symbol_o;
	symbol_o.setFont(font);
	symbol_o.setCharacterSize(30);
	symbol_o.setFillColor(sf::Color::Yellow);
	symbol_o.setStyle(sf::Text::Bold);
	symbol_o.setPosition(10, 140);
	if (ai_play)
	{
		if (self == 2) symbol_o.setString("O: Computer");
		else symbol_o.setString("O:" + player1);
	}
	else
	{
		if (player_x == 1) symbol_o.setString("O: " + player2);
		else symbol_o.setString("O: " + player1);
	}

	sf::Text chance_info;
	chance_info.setFont(font);
	chance_info.setCharacterSize(30);
	chance_info.setStyle(sf::Text::Bold);
	chance_info.setPosition(10, 250);

	sf::Text result;
	result.setFont(font);
	result.setCharacterSize(15);
	result.setStyle(sf::Text::Bold);
	result.setPosition(10, 315);

	sf::Texture x_symbol;
	x_symbol.loadFromFile("xo_data/x_symbol.png");
	sf::Texture o_symbol;
	o_symbol.loadFromFile("xo_data/o_symbol.png");

	sf::Music background;
	melody_again:
	int melody = (rand() % 5) + 1;
	if (melody > 5 || melody < 1) goto melody_again;
	background.openFromFile("xo_data/melody_" + std::to_string(melody) + ".wav");

	sf::SoundBuffer victory_buf;
	victory_buf.loadFromFile("xo_data/victory.wav");
	sf::Sound victory;
	victory.setBuffer(victory_buf);

	sf::SoundBuffer defeat_buf;
	defeat_buf.loadFromFile("xo_data/defeat.wav");
	sf::Sound defeat;
	defeat.setBuffer(defeat_buf);

	sf::SoundBuffer player_insert_buf;
	player_insert_buf.loadFromFile("xo_data/player_insert.wav");
	sf::Sound player_insert;
	player_insert.setBuffer(player_insert_buf);

	sf::SoundBuffer computer_insert_buf;
	computer_insert_buf.loadFromFile("xo_data/computer_insert.wav");
	sf::Sound computer_insert;
	computer_insert.setBuffer(computer_insert_buf);

	sf::SoundBuffer error_buf;
	error_buf.loadFromFile("xo_data/error.wav");
	sf::Sound error;
	error.setBuffer(error_buf);

	sf::SoundBuffer draw_buf;
	draw_buf.loadFromFile("xo_data/draw.wav");
	sf::Sound draw;
	draw.setBuffer(draw_buf);

	sf::RectangleShape symbols[9];
	for (int i = 0; i < 9; i++)
	{
		symbols[i].setSize(sf::Vector2f(80, 80));
		symbols[i].setTexture(&x_symbol);
	}
	symbols[0].setPosition(325, 100);
	symbols[1].setPosition(410, 100);
	symbols[2].setPosition(495, 100);
	symbols[3].setPosition(325, 185);
	symbols[4].setPosition(410, 185);
	symbols[5].setPosition(495, 185);
	symbols[6].setPosition(325, 270);
	symbols[7].setPosition(410, 270);
	symbols[8].setPosition(495, 270);

	int query;
	sf::RenderWindow window(sf::VideoMode(600, 375), "Tic Tac Toe: The Game");
	window.setFramerateLimit(10);
	window.requestFocus();
	background.play();

	while (window.isOpen())
	{
		query = 0;
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				background.stop();
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
					background.stop();
				}
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (MOUSE_X - 325 >= 0 && MOUSE_X - 325 <= 80)
				{
					if (MOUSE_Y - 100 >= 0 && MOUSE_Y - 100 <= 80) query = 1;
					else if (MOUSE_Y - 100 >= 85 && MOUSE_Y - 100 <= 165) query = 4;
					else if (MOUSE_Y - 100 >= 170 && MOUSE_Y - 100 <= 250) query = 7;
				}
				else if (MOUSE_X - 325 >= 85 && MOUSE_X - 325 <= 165)
				{
					if (MOUSE_Y - 100 >= 0 && MOUSE_Y - 100 <= 80) query = 2;
					else if (MOUSE_Y - 100 >= 85 && MOUSE_Y - 100 <= 165) query = 5;
					else if (MOUSE_Y - 100 >= 170 && MOUSE_Y - 100 <= 250) query = 8;
				}
				else if (MOUSE_X - 325 >= 170 && MOUSE_X - 325 <= 250)
				{
					if (MOUSE_Y - 100 >= 0 && MOUSE_Y - 100 <= 80) query = 3;
					else if (MOUSE_Y - 100 >= 85 && MOUSE_Y - 100 <= 165) query = 6;
					else if (MOUSE_Y - 100 >= 170 && MOUSE_Y - 100 <= 250) query = 9;
				}
			}
		}
		window.clear();

		if (ai_play && (chance % 2) == (self % 2))
		{
			delay(0.2);
			computer_insert.play();
			delay(1);
			int box_num = ai();
			box[box_num - 1] = self;
			if (self == 1) symbols[box_num - 1].setTexture(&x_symbol);
			else symbols[box_num - 1].setTexture(&o_symbol);
			chance++;
		}

		if (ai_play && query && (chance % 2) != (self % 2))
		{
			if (box[query - 1] == 0)
			{
				if (self == 1)
				{
					box[query - 1] = 2;
					symbols[query - 1].setTexture(&o_symbol);
					chance++;
				}
				else
				{
					box[query - 1] = 1;
					symbols[query - 1].setTexture(&x_symbol);
					chance++;
				}
				player_insert.play();
			}
			else error.play();
		}
		else if (!ai_play && query)
		{
			if (box[query - 1] == 0)
			{
				if (chance % 2 == 1)
				{
					box[query - 1] = 1;
					symbols[query - 1].setTexture(&x_symbol);
					chance++;
				}
				else
				{
					box[query - 1] = 2;
					symbols[query - 1].setTexture(&o_symbol);
					chance++;
				}
				player_insert.play();
			}
			else error.play();
		}

		if (ai_play)
		{
			if (chance % 2 == 1)
			{
				chance_info.setFillColor(sf::Color(0, 0, 200));
				if (self == 1)  chance_info.setString("Chance: Computer");
				else chance_info.setString("Chance: " + player1);
			}
			else
			{
				chance_info.setFillColor(sf::Color::Yellow);
				if (self == 2)  chance_info.setString("Chance: Computer");
				else chance_info.setString("Chance: " + player1);
			}
		}
		else
		{
			if (chance % 2 == 1)
			{
				chance_info.setFillColor(sf::Color(0, 0, 200));
				if (player_x == 1) chance_info.setString("Chance: " + player1);
				else chance_info.setString("Chance: " + player2);
			}
			else
			{
				chance_info.setFillColor(sf::Color::Yellow);
				if (player_x == 1) chance_info.setString("Chance: " + player2);
				else chance_info.setString("Chance: " + player1);
			}
		}

		if (query = checkGrid())
		{
			background.stop();
			std::string again_str = ".\nA new game will start soon\n(if you don't close the window)";
			if (query == 1)
			{
				if (ai_play)
				{
					if (self == 1)
					{
						result.setFillColor(sf::Color::Blue);
						result.setString("The computer won!" + again_str);
						defeat.play();
					}
					else
					{
						result.setFillColor(sf::Color::Yellow);
						result.setString("You won!" + again_str);
						victory.play();
					}
				}
				else
				{
					if (player_x == 1)
					{
						result.setFillColor(sf::Color::Blue);
						result.setString(player1 + " won!" + again_str);
					}
					else
					{
						result.setFillColor(sf::Color::Yellow);
						result.setString(player2 + " won!" + again_str);
					}
					victory.play();
				}
			}
			else if (query == 2)
			{
				if (ai_play)
				{
					if (self == 2)
					{
						result.setFillColor(sf::Color::Yellow);
						result.setString("The computer won!" + again_str);
						defeat.play();
					}
					else
					{
						result.setFillColor(sf::Color::Blue);
						result.setString("You won!" + again_str);
						victory.play();
					}
				}
				else
				{
					if (player_x == 2)
					{
						result.setFillColor(sf::Color::Yellow);
						result.setString(player2 + " won!" + again_str);
					}
					else
					{
						result.setFillColor(sf::Color::Blue);
						result.setString(player1 + " won!" + again_str);
					}
					victory.play();
				}
			}
			else if (query == 3)
			{
				result.setString("Game draw!" + again_str);
				draw.play();
			}
			window.draw(result);
			play_again = true;
		}

		window.draw(title);
		window.draw(symbol_x);
		window.draw(symbol_o);
		window.draw(chance_info);
		window.draw(grid);
		for (int i = 0; i < 9; i++) if (box[i]) window.draw(symbols[i]);
		window.display();

		if (play_again)
		{
			delay(3);
			for (int i = 0; i < 9; i++) box[i] = 0;
			window.close();
			main();
		}
	}
}

void init()
{
	if (play_again) goto forward;

	int choice;
	printf("---> Tic Tac Toe <---\n\n");
	printf("INSTRUCTIONS:\nTry to complete a row, column or diagonal with your symbol across the grid");
	printf("X always starts first. The player with symbol 'X' is randomly chosen");
	printf("Click on a blank square to insert your symbol\n\n");
	printf("BUGS:\nNone so far\n\n");
	printf("ABOUT:\nDeveloped by ChanRT\n\n");
	printf("Press (1) for a game with the computer or (2) for a two player game: ");
	std::cin >> choice;

	ai_play = true ? choice == 1 : false;

	if (!ai_play)
	{
		printf("\n\nEnter player 1's name: ");
		std::cin >> player1;
		printf("\nEnter player 2's name: ");
		std::cin >> player2;
	}
	else
	{
		printf("\n\nEnter your name: ");
		std::cin >> player1;
	}

	forward:

	int random = (rand() % 100) + 1;

	if (random % 2 == 0)
	{
		if (ai_play) self = 2;
		else player_x = 1;
	}
	else
	{
		if (ai_play) self = 1;
		else player_x = 2;
	}

	chance = 1;
}

void delay(float n_seconds)
{
	int milli = 1000 * n_seconds;

	clock_t start = clock();

	while (clock() < start + milli);
}