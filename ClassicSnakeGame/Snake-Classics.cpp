#define HEAD_X snake_x[0]
#define HEAD_Y snake_y[0]
#define HEAD snake_dir[0]
#define TAIL_X snake_x[len - 1]
#define TAIL_Y snake_y[len - 1]
#define TAIL snake_dir[len - 1]

#include<SFML/Audio.hpp>
#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Window.hpp>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "wtypes.h"

int level = 0, target;

int* snake_x, * snake_y, * snake_dir;
int len, score;
float snake_speed, displacement;

bool food_spawned;
int spawn_index;
int x, y;

int* bar_x, * bar_y;
int bars;

int horizontal, vertical;
int origin_x, origin_y;

bool back_play;

sf::Font font;
sf::Text title;

int menu();
void init();
void delay(float n_seconds);

void snakeGrow()
{
	if (TAIL == 1)
	{
		snake_x[len] = TAIL_X;
		snake_y[len] = TAIL_Y + 1;
	}
	else if (TAIL == 2)
	{
		snake_x[len] = TAIL_X;
		snake_y[len] = TAIL_Y - 1;
	}
	else if (TAIL == 3)
	{
		snake_x[len] = TAIL_X + 1;
		snake_y[len] = TAIL_Y;
	}
	else
	{
		snake_x[len] = TAIL_X - 1;
		snake_y[len] = TAIL_Y;
	}
	snake_dir[len] = TAIL;
	len++;
}

void moveSnake()
{
	int prev_dir = HEAD;
	int temp_dir;
	for (int i = 0; i < len; i++)
	{
		if (snake_dir[i] == 1) snake_y[i] = snake_y[i] - snake_speed;
		else if (snake_dir[i] == 2) snake_y[i] = snake_y[i] + snake_speed;
		else if (snake_dir[i] == 3) snake_x[i] = snake_x[i] - snake_speed;
		else snake_x[i] = snake_x[i] + snake_speed;

		if (snake_x[i] < 0) snake_x[i] = 79;
		if (snake_x[i] > 79) snake_x[i] = 0;
		if (snake_y[i] < 0) snake_y[i] = 59;
		if (snake_y[i] > 59) snake_y[i] = 0;

		if (snake_dir[i] != prev_dir)
		{
			temp_dir = snake_dir[i];
			snake_dir[i] = prev_dir;
			prev_dir = temp_dir;
		}
	}

	displacement = displacement + (snake_speed / 10);
}

void foodSpawn()
{
foodx_again:
	x = (rand() % 79) + 2;
	if (x > 79 || x < 2) goto foodx_again;

foody_again:
	y = (rand() % 59) + 2;
	if (y > 59 || y < 2) goto foody_again;

	for (int i = 0; i < len; i++)
	{
		if (x == snake_x[i] && y == snake_y[i]) goto foodx_again;
	}

	if (level)
	{
		for (int i = 0; i < bars; i++)
		{
			if (x == bar_x[i] && y == bar_y[i]) goto foodx_again;
		}
	}
	food_spawned = true;
	spawn_index = displacement;
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

int main()
{
	if (level <= 1) level = menu();
	srand(time(NULL));
	GetDesktopResolution(horizontal, vertical);
	init();

	printf("Level %d graphics loading\n", level);

	sf::RectangleShape field(sf::Vector2f(800, 600));
	field.setFillColor(sf::Color::Black);
	field.setPosition(origin_x, origin_y);

	printf("Play field loaded\n");

	sf::RectangleShape* segments = new sf::RectangleShape[target];
	segments[0].setSize(sf::Vector2f(10, 10));
	segments[0].setFillColor(sf::Color(192,192,192));
	for (int i = 1; i < target; i++)
	{
		segments[i].setSize(sf::Vector2f(10, 10));
		segments[i].setFillColor(sf::Color(255, 165, 0));
	}

	printf("Snake segments loaded\n");

	sf::RectangleShape* barriers = new sf::RectangleShape[bars];

	if (level)
	{
		for (int i = 0; i < bars; i++)
		{
			barriers[i].setSize(sf::Vector2f(10, 10));
			barriers[i].setFillColor(sf::Color::White);
			barriers[i].setOutlineThickness(2);
			barriers[i].setOutlineColor(sf::Color::Black);
			barriers[i].setPosition(origin_x + (10 * bar_x[i]), origin_y + (10 * bar_y[i]));
		}
	}
	printf("Barriers loaded\n");

	sf::CircleShape food(5);
	food.setFillColor(sf::Color::Red);

	sf::SoundBuffer food_spawn_buf;
	food_spawn_buf.loadFromFile("Snake_data/food_spawn.wav");
	sf::Sound food_spawn;
	food_spawn.setBuffer(food_spawn_buf);

	sf::SoundBuffer food_despawn_buf;
	food_despawn_buf.loadFromFile("Snake_data/food_despawn.wav");
	sf::Sound food_despawn;
	food_despawn.setBuffer(food_despawn_buf);

	sf::SoundBuffer snake_eat_buf;
	snake_eat_buf.loadFromFile("Snake_data/snake_eat.wav");
	sf::Sound snake_eat;
	snake_eat.setBuffer(snake_eat_buf);

	sf::SoundBuffer snake_bite_buf;
	snake_bite_buf.loadFromFile("Snake_data/snake_bite.wav");
	sf::Sound snake_bite;
	snake_bite.setBuffer(snake_bite_buf);

	sf::SoundBuffer next_level_buf;
	next_level_buf.loadFromFile("Snake_data/next_level.wav");
	sf::Sound next_level;
	next_level.setBuffer(next_level_buf);

	sf::SoundBuffer victory_buf;
	victory_buf.loadFromFile("Snake_data/victory.wav");
	sf::Sound victory;
	victory.setBuffer(victory_buf);

	sf::Music background;
	if (level == 0) background.openFromFile("Snake_data/level_0.wav");
	else if (level == 1) background.openFromFile("Snake_data/level_1.wav");
	else if (level == 2) background.openFromFile("Snake_data/level_2.wav");
	else if (level == 3) background.openFromFile("Snake_data/level_3.wav");

	printf("Music loaded\n");

	title.setPosition(origin_x + 280, 0);
	title.setFillColor(sf::Color::White);

	sf::Text mode;
	mode.setFont(font);
	mode.setCharacterSize(30);
	mode.setStyle(sf::Text::Bold);
	mode.setPosition(origin_x, origin_y - 35);
	if (level == 0) mode.setString("Mode: Infinite");
	else mode.setString("Mode: Arcade");

	sf::Text scoreboard;
	scoreboard.setFont(font);
	scoreboard.setCharacterSize(30);
	scoreboard.setStyle(sf::Text::Bold);
	scoreboard.setPosition(origin_x + 615, origin_y - 35);
	scoreboard.setString("Score: " + std::to_string(score));

	sf::Text levelname;
	levelname.setFont(font);
	levelname.setCharacterSize(30);
	levelname.setStyle(sf::Text::Bold);
	levelname.setPosition(origin_x, origin_y + 600);
	if (level == 0) levelname.setString("Level 0: 'Classic'; Limit = 480");
	else if (level == 1) levelname.setString("Level 1: 'Box'; Target = " + std::to_string(target));
	else if (level == 2) levelname.setString("Level 2: 'Cross'; Target = " + std::to_string(target));
	else levelname.setString("Level 3: '4 Houses'; Target = " + std::to_string(target));

	sf::Text length;
	length.setFont(font);
	length.setCharacterSize(30);
	length.setStyle(sf::Text::Bold);
	length.setPosition(origin_x + 625, origin_y + 600);
	length.setString("Length: " + std::to_string(len));

	printf("Text loaded\n");
	printf("Level %d will start in 2 seconds\n", level);
	delay(2);

	sf::RenderWindow window(sf::VideoMode(horizontal, vertical), "Snake Classics", sf::Style::Fullscreen);
	window.requestFocus();
	window.setFramerateLimit(10);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}
				if ((event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) && HEAD != 2) HEAD = 1;
				if ((event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) && HEAD != 1) HEAD = 2;
				if ((event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) && HEAD != 4) HEAD = 3;
				if ((event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) && HEAD != 3) HEAD = 4;
			}
		}

		moveSnake();
		for (int i = 0; i < len; i++)
		{
			segments[i].setPosition(origin_x + (snake_x[i] * 10), origin_y + (snake_y[i] * 10));
		}

		for (int i = 4; i < len; i++)
		{
			if (snake_x[i] == HEAD_X && snake_y[i] == HEAD_Y)
			{
				background.stop();
				snake_bite.play();
				delay(2);
				window.close();
			}
		}

		if (level)
		{
			for (int i = 0; i < bars; i++)
			{
				if (HEAD_X == bar_x[i] && HEAD_Y == bar_y[i])
				{
					background.stop();
					snake_bite.play();
					delay(2);
					window.close();
				}
			}
		}

		if (food_spawned)
		{
			if (HEAD_X == x && HEAD_Y == y)
			{
				score++;
				food_spawned = false;
				snakeGrow();
				score = score + (20 - (displacement - spawn_index));
				scoreboard.setString("Score: " + std::to_string(score));
				length.setString("Length: " + std::to_string(len));
				spawn_index = displacement;

				if (level)
				{
					if (len >= target)
					{
						background.stop();
						printf("Level escape triggered, window closed\n");
						delete(snake_x);
						delete(snake_y);
						delete(bar_x);
						delete(bar_y);
						level++;
						printf("Memory locations freed, level incremented\n");
						if (level < 4)
						{
							window.close();
							next_level.play();
							main();
						}
						else
						{
							victory.play();
							delay(5);
							window.close();
							exit(1);
						}
					}
				}
				snake_eat.play();
			}
		}

		if (!food_spawned && displacement - spawn_index > 3)
		{
			food_spawn.play();
			foodSpawn();
			food.setPosition(origin_x + (x * 10), origin_y + (y * 10));
		}
		if (food_spawned && displacement - spawn_index > 20)
		{
			food_despawn.play();
			food_spawned = false;
			spawn_index = displacement;
		}
		if (back_play == false && displacement >= 5)
		{
			background.play();
			back_play = true;
		}

		window.clear(sf::Color(0,0,128));
		window.draw(field);
		for (int i = 0; i < len; i++) window.draw(segments[i]);
		if (level) for (int i = 0; i < bars; i++) window.draw(barriers[i]);
		if (food_spawned) window.draw(food);

		window.draw(title);
		window.draw(mode);
		window.draw(scoreboard);
		window.draw(levelname);
		window.draw(length);

		window.display();
	}
}

void setBarriers()
{
	if (level == 1)
	{
		for (int i = 0; i < 80; i++)
		{
			bar_x[i] = i;
			bar_y[i] = 0;
		}
		for (int i = 80; i < 160; i++)
		{
			bar_x[i] = i - 80;
			bar_y[i] = 59;
		}
		for (int i = 160; i < 218; i++)
		{
			bar_x[i] = 0;
			bar_y[i] = i - 159;
		}
		for (int i = 218; i < 276; i++)
		{
			bar_x[i] = 79;
			bar_y[i] = i - 217;
		}
		printf("Barriers of level 1 initialised\n");
	}
	else if (level == 2)
	{
		for (int i = 0; i < 60; i++)
		{
			bar_x[i] = 40;
			bar_y[i] = i;
		}
		for (int i = 60; i < 140; i++)
		{
			bar_x[i] = i - 60; 
			bar_y[i] = 30;
		}
		printf("Barriers of level 2 initialised\n");
	}
	else if (level == 3)
	{
		for (int i = 0; i < 25; i++)
		{
			bar_x[i] = i;
			bar_y[i] = 25;
		}
		for (int i = 25; i < 50; i++)
		{
			bar_x[i] = i + 30;
			bar_y[i] = 25;
		}
		for (int i = 50; i < 75; i++)
		{
			bar_x[i] = i - 50;
			bar_y[i] = 35;
		}
		for (int i = 75; i < 100; i++)
		{
			bar_x[i] = i - 20;
			bar_y[i] = 35;
		}

		for (int i = 100; i < 125; i++)
		{
			bar_x[i] = 30;
			bar_y[i] = i - 99;
		}
		for (int i = 125; i < 150; i++)
		{
			bar_x[i] = 50;
			bar_y[i] = i - 124;
		}
		for (int i = 150; i < 175; i++)
		{
			bar_x[i] = 30;
			bar_y[i] = i - 115;
		}
		for (int i = 175; i < 200; i++)
		{
			bar_x[i] = 50;
			bar_y[i] = i - 140;
		}
	}
}

void init()
{
	printf("\nInitialising");

	int start_x, start_y, direction;
	int x_lower, x_upper, y_lower, y_upper, dir_lower, dir_upper;
	int start_length;

	origin_x = horizontal - 800;
	origin_x = origin_x / 2;
	origin_y = vertical - 600;
	origin_y = origin_y / 2;

	back_play = false;
	food_spawned = false;
	spawn_index = 0;

	snake_speed = 1;
	displacement = 0;

	if (level == 0)
	{
		printf(" level 0\n");
		x_upper = 50;
		x_lower = 30;
		y_upper = 35;
		y_lower = 25;
		dir_lower = 1;
		dir_upper = 4;

		start_length = 7;
		score = 0;
		bars = 0;
		target = 480;
	}
	else if (level == 1)
	{
		printf(" level 1\n");
		x_upper = 50;
		x_lower = 30;
		y_upper = 35;
		y_lower = 25;
		dir_upper = 4;
		dir_lower = 1;

		start_length = 10;
		score = 0;
		bars = 276;
		target = 30;
	}
	else if (level == 2)
	{
		printf(" level 2\n");
		x_upper = 70;
		x_lower = 55;
		y_upper = 29;
		y_lower = 1;
		dir_upper = 4;
		dir_lower = 3;

		start_length = 5;
		bars = 140;
		target = 20;
	}
	else if (level == 3)
	{
		printf(" level 3\n");
		x_upper = 49;
		x_lower = 31;
		y_upper = 33;
		y_lower = 27;
		dir_upper = 2;
		dir_lower = 1;

		start_length = 5;
		bars = 200;
		target = 20;
	}

	snake_x = new int[target];
	snake_y = new int[target];
	snake_dir = new int[target];
	printf("Snake array loaded\n");

	for (int i = 0; i < target; i++)
	{
		snake_x[i] = 0;
		snake_y[i] = 0;
		snake_dir[i] = 0;
	}

	bar_x = new int[bars];
	bar_y = new int[bars];
	printf("Barriers array loaded\n");

	setBarriers();

x_again:
	start_x = (rand() % x_upper) + x_lower;
	if (start_x < x_lower || start_x > x_upper) goto x_again;

y_again:
	start_y = (rand() % y_upper) + y_lower;
	if (start_y < y_lower || start_y > y_upper) goto y_again;

	printf("Snake coordinates determined\n");

dir_again:
	direction = (rand() % dir_upper) + dir_lower;
	if (direction < dir_lower || direction > dir_upper) goto dir_again;

	printf("Snake direction determined\n");

	len = start_length;
	
	if (direction == 1)
	{
		for (int i = 0; i < start_length; i++)
		{
			snake_x[i] = start_x;
			snake_y[i] = start_y + i;
			snake_dir[i] = 1;
		}
	}
	else if (direction == 2)
	{
		for (int i = 0; i < start_length; i++)
		{
			snake_x[i] = start_x;
			snake_y[i] = start_y - i;
			snake_dir[i] = 2;
		}
	}
	else if (direction == 3)
	{
		for (int i = 0; i < start_length; i++)
		{
			snake_x[i] = start_x + i;
			snake_y[i] = start_y;
			snake_dir[i] = 3;
		}
	}
	else
	{
		for (int i = 0; i < start_length; i++)
		{
			snake_x[i] = start_x - i;
			snake_y[i] = start_y;
			snake_dir[i] = 4;
		}
	}
	printf("Snake direction set, level %d initialisation complete\n\n", level);
}

int menu()
{
	font.loadFromFile("Snake_data/futura.ttf");
	title.setFont(font);
	title.setCharacterSize(40);
	title.setStyle(sf::Text::Bold);
	title.setString("Snake: Classics");
	title.setPosition(75, 0);
	title.setFillColor(sf::Color::Black);

	std::string instructions = "INSTRUCTIONS\nUse the Arrow keys to control the snake.\nEat food to grow and score\nFood spawns at random locations and\ndespawns after a fixed time\n\n";
	std::string bugs = "BUGS\nIf (suppose) the snake is moving forwards and\nthe player presses Right and Down in quick succession,\nthen the snake has as tendency to bite its own head.\nThis bug is hardware dependent.\n\n";
	std::string levels = "MODES\nTry the Arcade Mode to proceed through 3 levels\nwith different barries.\nOr play the Infinite Mode to experience\nthe classic snake game played on keypad phones\n\n";
	std::string about = "ABOUT\nDeveloped by ChanRT";
	
	sf::Text instruct;
	instruct.setFont(font);
	instruct.setCharacterSize(14);
	instruct.setStyle(sf::Text::Bold);
	instruct.setString(instructions + levels + bugs + about);
	instruct.setPosition(5, 70);
	instruct.setFillColor(sf::Color::Black);

	sf::Text arcade_mode;
	arcade_mode.setFont(font);
	arcade_mode.setCharacterSize(20);
	arcade_mode.setStyle(sf::Text::Bold);
	arcade_mode.setFillColor(sf::Color::Black);
	arcade_mode.setString("ARCADE");
	arcade_mode.setPosition(160, 452);

	sf::Text inf_mode;
	inf_mode.setFont(font);
	inf_mode.setCharacterSize(20);
	inf_mode.setStyle(sf::Text::Bold);
	inf_mode.setFillColor(sf::Color::Black);
	inf_mode.setString("INFINITE");
	inf_mode.setPosition(160, 522);

	sf::RectangleShape arcade_button(sf::Vector2f(250,30));
	arcade_button.setPosition(75, 450);
	arcade_button.setFillColor(sf::Color(211, 211, 211));

	sf::RectangleShape infinite_button(sf::Vector2f(250, 30));
	infinite_button.setPosition(75, 520);
	infinite_button.setFillColor(sf::Color(211, 211, 211));

	sf::RenderWindow start_menu(sf::VideoMode(400, 600), "Snake: Classics");
	start_menu.requestFocus();
	start_menu.setFramerateLimit(10);
	
	while (start_menu.isOpen())
	{
		sf::Event event;
		while (start_menu.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				start_menu.close();
				exit(1);
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (sf::Mouse::getPosition(start_menu).x >= 75 && sf::Mouse::getPosition(start_menu).x <= 325)
					{
						if (sf::Mouse::getPosition(start_menu).y >= 520 && sf::Mouse::getPosition(start_menu).y <= 550)
						{
							start_menu.close();
							return 0;
						}
						if (sf::Mouse::getPosition(start_menu).y >= 450 && sf::Mouse::getPosition(start_menu).y <= 480)
						{
							start_menu.close();
							return 1;
						}
					}
				}
			}
		}
		start_menu.clear(sf::Color::White);

		start_menu.draw(title);
		start_menu.draw(instruct);
		start_menu.draw(arcade_button);
		start_menu.draw(arcade_mode);
		start_menu.draw(infinite_button);
		start_menu.draw(inf_mode);
		
		start_menu.display();
	}
	return 0;
}

void delay(float n_seconds)
{
	int milli = 1000 * n_seconds;

	clock_t start = clock();

	while (clock() < start + milli);
}
