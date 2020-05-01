#include<SFML/Audio.hpp>
#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Window.hpp>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "wtypes.h"

bool ai_play;
bool target1, target2, deflecting;
bool up_left, up_right, down_left, down_right;
bool music_playing;

int horizontal, vertical;
int up_x, down_x, up_y, down_y;
int bar_speed;
int bounce;
int up_score, down_score;
int target_x;
int mouse_x;

float x, y;
float vx, vy;

sf::Text up_text;
std::string up_subtext;

sf::Text down_text;
std::string down_subtext;

sf::Text score;
std::string score_subtext;

sf::Music back;
sf::Sound bar_hit;
sf::Sound wall_hit;
sf::Sound fail;

void instructions();
void delay(float n_seconds = 1);

void ai()
{
	if (vy > 0 && target1 == false)
	{
		float temp_x = x, temp_y = y, temp_vx = vx, temp_vy = vy;
		while (temp_y > (up_y + 5))
		{
			temp_x = temp_x + temp_vx;
			temp_y = temp_y + temp_vy;

			if (temp_x >= horizontal - 12 || temp_x <= 12)
			{
				temp_vx = -temp_vx;
			}

			if (temp_y >= down_y - 6)
			{
				temp_vy = -temp_vy;
			}
		}
		target_x = temp_x;
		target1 = true;
	}

	if (vy < 0 && target2 == false)
	{
		float temp_x = x, temp_y = y, temp_vx = vx, temp_vy = vy;
		while (temp_y > (up_y + 5))
		{
			temp_x = temp_x + temp_vx;
			temp_y = temp_y + temp_vy;

			if (temp_x >= horizontal - 10 || temp_x <= 10)
			{
				temp_vx = -temp_vx;
			}

			if (temp_y >= down_y - 6)
			{
				temp_vy = -temp_vy;
			}
		}
		target_x = temp_x;
		target2 = true;
	}
}

void gameLogic()
{
	if (up_left && up_x >= 100 && !ai_play) up_x = up_x - bar_speed;
	if (up_right && up_x <= horizontal - 100 && !ai_play) up_x = up_x + bar_speed;
	if (down_left && down_x >= 100) down_x = down_x - bar_speed;
	if (down_right && down_x <= horizontal - 100) down_x = down_x + bar_speed;

	x = x + vx;
	y = y + vy;

	if (x >= horizontal - 12 || x <= 12)
	{
		if (x >= horizontal - 12) x = horizontal - 12;
		else x = 12;
		vx = -vx;
		wall_hit.play();
	}

	if (y <= up_y + 5 && y >= up_y - 10)
	{
		if (x >= up_x - 50 && x <= up_x + 50)
		{
			vy = -vy;
			y = up_y + 6;
			if (!ai_play) bounce++;
			bar_hit.play();
		}
		if (x >= up_x - 100 && x < up_x - 50)
		{
			vy = -vy;
			y = up_y + 6;
			if (vx > 0) vx = -7;
			else vx = -9;
			if (!ai_play) bounce++;
			bar_hit.play();
		}
		if (x > up_x + 50 && x < up_x + 100)
		{
			vy = -vy;
			y = up_y + 6;
			if (vx < 0) vx = 7;
			else vx = 9;
			if (!ai_play) bounce++;
			bar_hit.play();
		}
		target1 = false;
		score.setString(score_subtext + std::to_string(bounce));
	}
	if (y >= down_y - 5 && y <= down_y + 10)
	{
		if (x >= down_x - 50 && x <= down_x + 50)
		{
			vy = -vy;
			y = down_y - 6;
			bounce++;
			bar_hit.play();
		}
		if (x >= down_x - 100 && x < down_x - 50)
		{
			vy = -vy;
			y = down_y - 6;
			if (vx > 0) vx = -7;
			else vx = -9;
			bounce++;
			bar_hit.play();
		}
		if (x > down_x + 50 && x < down_x + 100)
		{
			vy = -vy;
			y = down_y - 6;
			if (vx < 0) vx = 7;
			else vx = 9;
			bounce++;
			bar_hit.play();
		}
		target2 = false;
		deflecting = false;
		score.setString(score_subtext + std::to_string(bounce));
	}

	if (y <= 20 || y >= vertical - 20)
	{
		back.stop();
		music_playing = false;

		bounce = 0;
		vx = 0;
		vy = 0;
		fail.play();

		if (y <= 20)
		{
			down_score++;
			down_text.setString(down_subtext + std::to_string(down_score));
		}
		else
		{
			up_score++;
			up_text.setString(up_subtext + std::to_string(up_score));
		}
		score.setString(score_subtext + "0");
		delay(2);

	spawn_again:
		x = (rand() % int(horizontal / 2 + 200)) + int(horizontal / 2 - 200);
		if (x > (horizontal / 2 + 200) || x < (horizontal / 2 - 200)) goto spawn_again;
		y = vertical / 4;
		vx = (-8) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 16));
		vy = 8.5;

		up_x = horizontal / 2;
		down_x = up_x;
	}

	if (bounce > 40)
	{
		if (vy < 0) vy = -13.0;
		else vy = 13.0;
	}
	else if (bounce > 35)
	{
		if (vy < 0) vy = -12.7;
		else vy = 12.7;
	}
	else if (bounce > 30)
	{
		if (vy < 0) vy = -12.4;
		else vy = 12.4;
	}
	else if (bounce > 25)
	{
		if (vy < 0) vy = -12;
		else vy = 12;
	}
	else if (bounce > 20)
	{
		if (vy < 0) vy = -11.5;
		else vy = 11.5;
	}
	else if (bounce > 15)
	{
		if (vy < 0) vy = -10.9;
		else vy = 10.9;
	}
	else if (bounce > 10)
	{
		if (vy < 0) vy = -10.2;
		else vy = 10.2;
	}
	else if (bounce > 4)
	{
		if (vy < 0) vy = -9.3;
		else vy = 9.3;
	}
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
	srand(time(NULL));
	GetDesktopResolution(horizontal, vertical);

	ai_play = true;
	target1 = false;
	target2 = false;
	music_playing = false;

	std::cout << "DETECT Screen resolution " << horizontal << "x" << vertical << std::endl << std::endl;
	instructions();
	
	up_left = false;
	up_right = false;
	down_left = false;
	down_right = false;

	up_x = horizontal / 2;
	up_y = 70;
	down_x = up_x;
	down_y = vertical - 70;
	
	if (horizontal < 1000) bar_speed = 8;
	else if (horizontal >= 1000 && horizontal < 1300) bar_speed = 10;
	else if (horizontal >= 1300 && horizontal < 1500) bar_speed = 11;
	else bar_speed = 12;

	bounce = 0;
	up_score = 0;
	down_score = 0;

	x_again:
	x = (rand() % int(horizontal / 2 + 200)) + int(horizontal / 2 - 200);
	if (x > (horizontal / 2 + 200) || x < (horizontal / 2 - 200)) goto x_again;
	y = vertical / 4;

	vx = (-8) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 16));
	vy = 8.5;
    
	sf::RectangleShape bar_up(sf::Vector2f(200, 20));
	bar_up.setOrigin(100, 20);
	bar_up.setPosition(up_x, up_y);
	bar_up.setFillColor(sf::Color::White);

	sf::RectangleShape bar_down(sf::Vector2f(200, 20));
	bar_down.setOrigin(100, 0);
	bar_down.setPosition(down_x, down_y);
	bar_down.setFillColor(sf::Color::White);

	sf::CircleShape ball(10);
	ball.setOrigin(5, 5);
	ball.setPosition(x, y);
	ball.setFillColor(sf::Color::White);

	sf::Font font;
	font.loadFromFile("Bounce_data/futura.ttf");

	up_text.setFont(font);
	up_text.setCharacterSize(40);
	up_text.setFillColor(sf::Color::White);
	up_text.setStyle(sf::Text::Bold);
	up_text.setPosition(0, 0);
	if (ai_play) up_subtext = "Computer: ";
	else up_subtext = "Player 1: ";

	down_text.setFont(font);
	down_text.setCharacterSize(35);
	down_text.setFillColor(sf::Color::White);
	down_text.setStyle(sf::Text::Bold);
	down_text.setPosition(0, vertical - 42);
	if (ai_play) down_subtext = "You: ";
	else down_subtext = "Player 2: ";

	sf::Text title;
    title.setFont(font);
    title.setCharacterSize(40);
	title.setFillColor(sf::Color::White);
	title.setStyle(sf::Text::Bold);
	title.setString("Bounce");
	title.setPosition((horizontal / 2) - 60, 0);

	score.setFont(font);
	score.setCharacterSize(40);
	score.setFillColor(sf::Color::White);
    score.setStyle(sf::Text::Bold);
	score.setPosition(horizontal - 200, 0);
	score_subtext = "Score: ";

	up_text.setString(up_subtext + "0");
	down_text.setString(down_subtext + "0");
	score.setString(score_subtext + "0");

	sf::SoundBuffer bar_hit_buf;
	bar_hit_buf.loadFromFile("Bounce_data/bar_hit.wav");
	bar_hit.setBuffer(bar_hit_buf);

	sf::SoundBuffer wall_hit_buf;
	wall_hit_buf.loadFromFile("Bounce_data/wall_hit.wav");
	wall_hit.setBuffer(wall_hit_buf);

	sf::SoundBuffer fail_buf;
	fail_buf.loadFromFile("Bounce_data/fail.wav");
	fail.setBuffer(fail_buf);
	back.openFromFile("Bounce_data/electro_pop.wav");

	sf::RenderWindow window(sf::VideoMode(horizontal, vertical), "Bounce",sf::Style::Fullscreen);
	window.setFramerateLimit(30);
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
				if (event.key.code == sf::Keyboard::Left)
				{
					down_left = true;
				}
				if (event.key.code == sf::Keyboard::Right)
				{
					down_right = true;
				}
				if (event.key.code == sf::Keyboard::A)
				{
					up_left = true;
				}
				if (event.key.code == sf::Keyboard::D)
				{
					up_right = true;
				}
			}
			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Left)
				{
					down_left = false;
				}
				if (event.key.code == sf::Keyboard::Right)
				{
					down_right = false;
				}
				if (event.key.code == sf::Keyboard::A)
				{
					up_left = false;
				}
				if (event.key.code == sf::Keyboard::D)
				{
					up_right = false;
				}
			}
		}

		gameLogic();
		if (ai_play)
		{
			if (vy < 0 && y < (vertical / 2) && target_x - 10 < up_x && up_x < target_x + 10 && !deflecting)
			{
				if (up_x > down_x)
				{
					target_x = target_x - 70;
					deflecting = true;
				}
				if (up_x < down_x)
				{
					target_x = target_x + 70;
					deflecting = true;
				}
			}

			if ((vy > 0 && target1 == false) || (vy < 0 && target2 == false)) ai();
			else if (!(target_x - 10 < up_x && up_x < target_x + 10))
			{
				if (target_x > up_x && up_x <= horizontal - 100) up_x = up_x + bar_speed;
				if (target_x < up_x && up_x >= 100) up_x = up_x - bar_speed;
			}
		}

		if (bounce >= 3 && !music_playing)
		{
			back.play();
			music_playing = true;
		}

		bar_up.setPosition(up_x, up_y);
		bar_down.setPosition(down_x, down_y);
		ball.setPosition(x, y);

		window.clear();

		window.draw(bar_up);
		window.draw(bar_down);
		window.draw(ball);

		window.draw(title);
		window.draw(up_text);
		window.draw(down_text);
		window.draw(score);

		window.display();
	}
}

void instructions()
{
	int choice;

	std::cout << "---> Bounce Game <---" << std::endl << std::endl;

	std::cout << "INSTRUCTIONS:" << std::endl;
	std::cout << "Prevent the ball from falling in through your side" << std::endl;
	std::cout << "As a player, use Left and Right cursor keys to steer your bar (present at the bottom)" << std::endl;
	std::cout << "In two player mode, the second player should use A and D to steer the bar (present at the top)" << std::endl << std::endl;
	std::cout << "Hit the ball with the left end of your bar to deflect it towards the left, and vice versa" << std::endl;
	std::cout << "The computer also uses deflection to try and score a point" << std::endl << std::endl;

	std::cout << "BUGS:" << std::endl;
	std::cout << "1. The ball is a blur, or wobbles in the middle of the screen; This happens due to a big difference in the" << std::endl;
	std::cout << "horizontal and vertical velocities of the ball, coupled with a slow framerate of 30 fps" << std::endl;
	std::cout << "2. The ball get trapped in the edge of the screen. Reason unknown. Just restart the game" << std::endl << std::endl;

	std::cout << "ABOUT:" << std::endl;
	std::cout << "Game developed by ChanRT" << std::endl << std::endl;

	std::cout << "What would you like to play ?" << std::endl << std::endl;
	std::cout << "1: Play against Computer" << std::endl;
	std::cout << "2: Two Player" << std::endl << std::endl;
	std::cout << "Your choice: ";
	std::cin >> choice;
	std::cout << std::endl;
	std::cout << "Loading. Please Wait.";

	ai_play = true ? choice == 1 : false;
}

void delay(float n_seconds)
{
	int milli = 1000 * n_seconds;

	clock_t start = clock();

	while (clock() < start + milli);
}