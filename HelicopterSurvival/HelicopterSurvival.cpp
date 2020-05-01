#include<SFML/Audio.hpp>
#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Window.hpp>
#include <iostream>
#include <ctime>
#include <conio.h>
#include "wtypes.h"

// Cleanest code I've ever written !

int horizontal, vertical, fps;
int bg_speed, background_x[2];
int game_cycle, cycle_stage;

int x, y, front_x, top_y, bottom_y;

int max_num_walls;
int wall_thickness;

class Wall
{
private:
	int x, y1, y2;
	bool exist;

public:
	Wall()
	{
		exist = false;
	}

	void createNewWall(int gap)
	{
		x = horizontal;

		gen_y1_again:
		y1 = (rand() % vertical - gap) + gap;
		if (y1 > vertical - gap - 120 || y1 < gap) goto gen_y1_again;
		y2 = y1 + gap;

		exist = true;
	}

	bool exists()
	{
		return exist;
	}

	void move(int speed)
	{
		x = x - speed;
		if (x < -wall_thickness)
		{
			exist = false;
		}
	}

	int getWallCoord()
	{
		return x;
	}

	int getUpperWall()
	{
		return y1;
	}

	int getLowerWall()
	{
		return y2;
	}

	int getUpperWallSize()
	{
		return y1;
	}

	int getLowerWallSize()
	{
		return (vertical - y2);
	}

	void destroy()
	{
		exist = false;
	}
};

Wall* walls;
sf::RectangleShape* wall_rects;

class Helis
{
private:
	int heli_x, heli_y;
	bool exist;

public:
	Helis()
	{
		exist = false;
	}

	void createNewHeli()
	{
		heli_x = horizontal;
		gen_heli_y_again:
		heli_y = (rand() % vertical - 150) + 20;
		if (heli_y > vertical - 150 || heli_y < 20) goto gen_heli_y_again;
		exist = true;
	}

	bool exists()
	{
		return exist;
	}

	void destroy()
	{
		exist = false;
	}

	void move()
	{
		heli_x = heli_x - 1.25 * bg_speed;
		
		if (horizontal / 2 < heli_x && heli_x < 3 * horizontal / 4)
		{
			if (heli_y > y) heli_y = heli_y - 4;
			else if (heli_y < y) heli_y = heli_y + 4;
		}
		if (heli_x + 68 < 0) exist = false;
	}

	int getHeliCoord()
	{
		return heli_x;
	}

	int getHeliUpper()
	{
		return heli_y;
	}
	int getHeliLower()
	{
		return heli_y + 20;
	}
};

int max_num_helis;
Helis* ai_helis;
sf::RectangleShape* ai_heli_rects;

int helis_to_be_created, walls_to_be_created;
bool up_press, down_press, left_press, right_press;
bool exploded;
int exp_i, exp_j;

void delay(float n_seconds);
void init();

void createWall()
{
	int wall_index;
	for (wall_index = 0; wall_index < max_num_walls; wall_index++)
	{
		if (walls[wall_index].exists() == false) break;
	}
	walls[wall_index].createNewWall(120);

	wall_rects[2 * wall_index].setSize(sf::Vector2f(wall_thickness, walls[wall_index].getUpperWallSize()));
	wall_rects[2 * wall_index + 1].setSize(sf::Vector2f(wall_thickness, walls[wall_index].getLowerWallSize()));
}

void createHeli()
{
	int heli_index;
	for (heli_index = 0; heli_index < max_num_helis; heli_index++)
	{
		if (ai_helis[heli_index].exists() == false) break;
	}
	ai_helis[heli_index].createNewHeli();
}

void createEntities()
{
	if (game_cycle == 2)
	{
		walls_to_be_created = 3;
	}
	if (walls_to_be_created)
	{
		createWall();
		walls_to_be_created--;
	}
	else if (helis_to_be_created)
	{
		createHeli();
		helis_to_be_created--;
	}
	else
	{
		int create = (rand() % 6) + 1;
		int num = (rand() % 4) + 2;

		if (create == 1 || create == 2) createHeli();
		else if (create == 3) createWall();
		else if (create == 4)
		{
			createHeli();
			helis_to_be_created = num;
		}
		else 
		{
			createWall();
			walls_to_be_created = num;
		}
	}
	if (game_cycle % 20 == 0 && cycle_stage == 0) bg_speed++;
}

void moveBackground()
{
	for (int i = 0; i < 2; i++)
	{
		background_x[i] = background_x[i] - bg_speed;
		if (background_x[i] - 2 * bg_speed<= -horizontal) background_x[i] = horizontal - 2 * bg_speed;
	}
}

void moveHeli()
{
	if (up_press && y > 20) y = y - 4;
	else if (y < vertical - 120) y = y + 4;
	if (down_press && y < vertical - 120) y = y + 4;
	if (right_press && x < 3 * horizontal / 4) x = x + 3;
	else if (x > 20) x = x - 4;
	if (left_press && x > 20) x = x - 3;

	front_x = x + 68;
	top_y = y;
	bottom_y = y + 25;
}

void moveHelisAndSetPositions()
{
	for (int i = 0; i < max_num_helis; i++)
	{
		if (ai_helis[i].exists())
		{
			ai_helis[i].move();
			ai_heli_rects[3 * i].setPosition(ai_helis[i].getHeliCoord(), ai_helis[i].getHeliUpper());
			ai_heli_rects[3 * i + 1].setPosition(ai_helis[i].getHeliCoord(), ai_helis[i].getHeliUpper());
			ai_heli_rects[3 * i + 2].setPosition(ai_helis[i].getHeliCoord(), ai_helis[i].getHeliUpper());
		}
	}
}

void moveWallsAndSetPositions()
{
	for (int i = 0; i < max_num_walls; i++)
	{
		if (walls[i].exists())
		{
			walls[i].move(bg_speed);
			wall_rects[2 * i].setPosition(walls[i].getWallCoord(), 0);
			wall_rects[2 * i + 1].setPosition(walls[i].getWallCoord(), walls[i].getLowerWall());
		}
	}
}

bool collissionWithWall()
{
	for (int i = 0; i < max_num_walls; i++)
	{
		if (walls[i].exists())
		{
			if (front_x >= walls[i].getWallCoord() && front_x < walls[i].getWallCoord() + wall_thickness + 40)
			{
				if (top_y <= walls[i].getUpperWall() || bottom_y >= walls[i].getLowerWall()) return true;
			}
			else if (front_x >= walls[i].getWallCoord() + wall_thickness + 40 && front_x < walls[i].getWallCoord() + wall_thickness + 68)
			{
				if (top_y <= walls[i].getUpperWall() || bottom_y - 20 >= walls[i].getLowerWall()) return true;
			}
		}
	}
	return false;
}

bool collissionWithHeli()
{
	for (int i = 0; i < max_num_helis; i++)
	{
		if (ai_helis[i].exists())
		{
			if (front_x >= ai_helis[i].getHeliCoord() && x <= ai_helis[i].getHeliCoord() + 68)
			{
				if (top_y < ai_helis[i].getHeliLower() && ai_helis[i].getHeliLower() < bottom_y) return true;
				if (top_y < ai_helis[i].getHeliUpper() && ai_helis[i].getHeliUpper() < bottom_y) return true;
			}
		}
	}
	return false;
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

void gameLoop()
{
	srand(time(NULL));

	sf::Texture bg_texture;
	bg_texture.loadFromFile("heli_data/grass_bg.png");

	sf::RectangleShape backgrounds[2];
	for (int i = 0; i < 2; i++)
	{
		backgrounds[i].setSize(sf::Vector2f(horizontal, vertical));
		backgrounds[i].setTexture(&bg_texture);
	}

	sf::Texture heli_texture[3];
	for (int i = 0; i < 3; i++) heli_texture[i].loadFromFile("heli_data/heli_" + std::to_string(i + 1) + ".png");

	sf::RectangleShape heli[3];
	for (int i = 0; i < 3; i++)
	{
		heli[i].setSize(sf::Vector2f(68, 40));
		heli[i].setTexture(&heli_texture[i]);
		heli[i].setOrigin(68, 0);
		heli[i].setScale(sf::Vector2f(-1, 1));
	}

	ai_heli_rects = new sf::RectangleShape[3 * max_num_helis];
	for (int i = 0; i < 3 * max_num_helis; i++)
	{
		ai_heli_rects[i].setSize(sf::Vector2f(68, 40));
		ai_heli_rects[i].setTexture(&heli_texture[i % 3]);
	}

	sf::Texture wall_texture;
	wall_texture.setRepeated(true);
	wall_texture.loadFromFile("heli_data/brown_square.png");

    wall_rects = new sf::RectangleShape[2 * max_num_walls];
	for (int i = 0; i < 2 * max_num_walls; i++) wall_rects[i].setTexture(&wall_texture);

	sf::Texture exploding_parent;
	exploding_parent.loadFromFile("heli_data/explosion.png");

	sf::Sprite exploding_animation;
	exploding_animation.setTexture(exploding_parent);
	
	sf::SoundBuffer helisound_buf;
	helisound_buf.loadFromFile("heli_data/helisound.wav");

	sf::Sound helisound;
	helisound.setBuffer(helisound_buf);
	helisound.setVolume(40);
	helisound.setLoop(true);
	helisound.play();

	sf::SoundBuffer explosion_buf;
	explosion_buf.loadFromFile("heli_data/explosion.wav");

	sf::Sound explosion;
	explosion.setBuffer(explosion_buf);

	sf::Music bg_music;
	bg_music.openFromFile("heli_data/background.wav");
	bg_music.play();
	bg_music.setLoop(true);

	sf::RenderWindow window(sf::VideoMode(horizontal, vertical), "Helicopter Survival", sf::Style::Fullscreen);
	window.setFramerateLimit(fps);
	window.requestFocus();

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
				if (event.key.code == sf::Keyboard::Up)
				{
					helisound.setVolume(60);
					helisound.setPitch(1.15);
					up_press = true;
				}
				if (event.key.code == sf::Keyboard::Down)
				{
					helisound.setVolume(30);
					helisound.setPitch(0.8);
					down_press = true;
				}
				if (event.key.code == sf::Keyboard::Left)
				{
					left_press = true;
				}
				if (event.key.code == sf::Keyboard::Right)
				{
					right_press = true;
				}
			}
			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Up)
				{
					helisound.setVolume(40);
					helisound.setPitch(1);
					up_press = false;
				}
				if (event.key.code == sf::Keyboard::Down)
				{
					helisound.setVolume(40);
					helisound.setPitch(1);
					down_press = false;
				}
				if (event.key.code == sf::Keyboard::Left)
				{
					left_press = false;
				}
				if (event.key.code == sf::Keyboard::Right)
				{
					right_press = false;
				}
			}
		}
		window.clear();

		if(game_cycle % 2 == 0 && cycle_stage == fps / 2) createEntities();

		moveBackground();
		moveHeli();
		moveWallsAndSetPositions();
		moveHelisAndSetPositions();

		if (collissionWithWall() || collissionWithHeli() && !exploded)
		{
			helisound.stop();
			bg_music.stop();
			explosion.play();
			exploded = true;
			exploding_animation.setPosition(x, y);
		}

		if (exploded)
		{
			exploding_animation.setTextureRect(sf::IntRect(exp_i, exp_j, 100, 100));
			exp_i = exp_i + 100;
			if (exp_i > 400)
			{
				exp_i = 0;
				exp_j = exp_j + 100;
			}
			if (exp_j > 400)
			{
				window.close();
				system("cls");
				printf("\nPRESS ANY KEY TO PLAY AGAIN (Or else, close the window)");
				_getch();
				init();
			}
		}

		for (int i = 0; i < 2; i++) backgrounds[i].setPosition(background_x[i], 0);
		heli[cycle_stage % 3].setPosition(x, y);

		for (int i = 0; i < 2; i++) window.draw(backgrounds[i]);
		if(!exploded) window.draw(heli[cycle_stage % 3]);
		for (int i = 0; i < max_num_walls; i++)
		{
			if (walls[i].exists())
			{
				window.draw(wall_rects[2 * i]);
				window.draw(wall_rects[2 * i + 1]);
			}
		}
		for (int i = 0; i < max_num_helis; i++)
		{
			if (ai_helis[i].exists())
			{
				window.draw(ai_heli_rects[(3 * i) + (cycle_stage % 3)]);
			}
		}

		if (exploded)
		{
			exploding_animation.setPosition(x, y);
			window.draw(exploding_animation);
		}
		
		window.display();

		cycle_stage++;
		if (cycle_stage > fps)
		{
			cycle_stage = 0;
			game_cycle++;
		}
	}
}

void init()
{
	cycle_stage = 0;
	game_cycle = 0;

	bg_speed = 5;
	background_x[0] = 0;
	background_x[1] = horizontal - 2 * bg_speed;

	fps = 30;
	up_press = down_press = left_press = right_press = false;

	x = 100;
	y = vertical / 2;

	max_num_walls = 8;
	wall_thickness = 20;
	walls = new Wall[max_num_walls];

	max_num_helis = 8;
	ai_helis = new Helis[max_num_helis];

	helis_to_be_created = 0;
	walls_to_be_created = 0;

	exploded = false;
	exp_i = exp_j = 0;

	gameLoop();
}

int main()
{
	printf("---> Helicopter Survival <---\n\n");
	printf("INSTRUCTIONS\n");
	printf("Use the arrow keys to control your helicopter\n");
	printf("If you collide with a wall or another helicopter, it's game over\n");
	printf("The obstacles are randomly generated and the speed gets faster with time\n\n");
	printf("BUGS\n");
	printf("A small discontintuity in background arises whenever the speed changes\n\n");
	printf("ABOUT\n");
	printf("Developed by ChanRT\n");
	printf("Images of the helicopter, background and explosion were taken from a free Android game dev tutorial\n\n");
	printf("PRESS ANY KEY TO BEGIN");
	_getch();
	printf("\nLoading, please wait");
	GetDesktopResolution(horizontal, vertical);
	init();
}

void delay(float n_seconds)
{
	int milli = 1000 * n_seconds;

	clock_t start = clock();

	while (clock() < start + milli);
}