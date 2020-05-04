#include<SFML/Audio.hpp>
#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Window.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <conio.h>
#include "wtypes.h"

int horizontal, vertical, back_x, back_y, back_len, back_wid;
int game_cycle, cycle_stage, fps, move_cycle;
int wave, target, zombie_spawn_cycle, zombie_spawn_stage;

int localX(int global)
{
	return (global - back_len / 2 + horizontal / 2);
}

int localY(int global)
{
	return (global - back_wid / 2 + vertical / 2);
}

int local_x, local_y;
int player_speed, player_health, score;
float initial_angle, desired_angle;

bool up_key, down_key, left_key, right_key;
bool mouse_left_pressed, mouse_left_released;

int target_x, target_y;
int bullet_speed, max_num_bullets;
bool targeting, aim_while_moving;

class Bullet
{
private:
	float x, y, vx, vy;
	bool exist;

public:
	Bullet()
	{
		exist = false;
	}

	void createBullet()
	{
		float angle_in_radian = 3.14 * initial_angle / 180;

		x = local_x + 80 * sin(angle_in_radian);
		y = local_y - 80 * cos(angle_in_radian);

		vy = bullet_speed * cos(angle_in_radian);
		vx = bullet_speed * sin(angle_in_radian);
		exist = true;

		if (target_x - local_x > 0) vx = abs(vx);
		else vx = -abs(vx);
		if (target_y - local_y > 0) vy = abs(vy);
		else vy = -abs(vy);
	}

	bool exists()
	{
		return exist;
	}

	void move()
	{
		x = x + vx;
		y = y + vy;

		if (x > horizontal || x < 0 || y > vertical || y < 0) exist = false;
	}

	void destroy()
	{
		exist = false;
	}

	float getX()
	{
		return x;
	}

	float getY()
	{
		return y;
	}
};

Bullet* bullets;
sf::CircleShape* bullet_circles;

int zombie_speed, max_num_zombies;
sf::Texture* zombie_move;
sf::Texture* zombie_attack;
sf::Sound zombie_die, zombie_attack_sound, player_hurt, first_aid_sound;

bool first_aid_spawned;
int aid_x, aid_y;

class Zombie
{
private:
	float x, y, vx, vy;
	int hp, cycle;
	bool exist, attacking;
	float initial_angle, desired_angle;

	void calibrate()
	{
		if (abs(localX(x) - local_x) > 30)
		{
			if (local_x > localX(x)) vx = zombie_speed;
			else vx = -zombie_speed;
		}
		else vx = 0;

		if (abs(localY(y) - local_y) > 30)
		{
			if (local_y > localY(y)) vy = zombie_speed;
			else vy = -zombie_speed;
		}
		else vy = 0;

		if (vx == 0)
		{
			if (vy > 0) desired_angle = 90;
			else desired_angle = 270;
		}
		else if (vy == 0)
		{
			if (vx > 0) desired_angle = 0;
			else desired_angle = 180;
		}
		else
		{
			if (vx > 0 && vy > 0) desired_angle = 45;
			else if (vx > 0 && vy < 0) desired_angle = 315;
			else if (vx < 0 && vy > 0) desired_angle = 135;
			else if (vx < 0 && vy < 0) desired_angle = 215;
		}
	}

	bool checkAttack()
	{
		return (abs(localX(x) - local_x) < 80 && abs(localY(y) - local_y) < 80);
	}

public:
	Zombie()
	{
		exist = attacking = false;
		hp = cycle = 0;
		desired_angle = initial_angle = 0;
	}

	void createZombie()
	{
		gen_x_again:
		x = (rand() % back_len - 100) + 100;
		if (abs(localX(x) - local_x) < horizontal / 2) goto gen_x_again;

		gen_y_again:
		y = (rand() % back_wid - 100) + 100;
		if (abs(localY(y) - local_y) < vertical / 2) goto gen_y_again;

		hp = 2;
		exist = true;

		calibrate();
	}

	void move(int move_x, int move_y)
	{
		if (x > 75 && x < back_len - 75 && y > 75 && y < back_wid - 75)
		{
			x = x + move_x;
			y = y + move_y;
		}
	}

	void move()
	{
		if (!attacking && x > 75 && x < back_len - 75 && y > 75 && y < back_wid - 75)
		{
			x = x + vx;
			y = y + vy;
		}
		cycle++;
		if (cycle > 8)
		{
			cycle = 0;
			if (x < 0 || x > back_len || y < 0 || y > back_wid) exist = false;
		}

		if(!attacking && cycle == 0) calibrate();
		if (attacking && cycle == 8)
		{
			player_hurt.play();
			attacking = false;
			player_health = player_health - 10;
		}
		if (!attacking && cycle == 8 && checkAttack())
		{
			zombie_attack_sound.play();
			cycle = 0;
			attacking = true;
		}
	}

	void kill()
	{
		exist = false;
	}

	bool inFocus()
	{
		if (exist && localX(x) > 0 && localX(x) < horizontal)
		{
			if (localY(y) > 0 && localY(y) < vertical) return true;
		}
		return false;
	}

	bool exists()
	{
		return exist;
	}

	bool isAttacking()
	{
		return attacking;
	}

	void shot()
	{
		hp--;
		if (hp < 0)
		{
			zombie_die.play();
			score++;
			exist = false;

			if (!first_aid_spawned)
			{
				int spawn_aid = (rand() % 10) + 1;
				if (spawn_aid == 1)
				{
					first_aid_sound.play();
					first_aid_spawned = true;
					aid_x = localX(x);
					aid_y = localY(y);
				}
			}
		}
	}

	int getCycle()
	{
		if (cycle > 8) return 8;
		else return cycle;
	}

	float getX()
	{
		return localX(x);
	}

	float getY()
	{
		return localY(y);
	}

	float getInitialAngle()
	{
		return initial_angle;
	}

	float getDesiredAngle()
	{
		return desired_angle;
	}

	void setInitialAngle(float init_angle)
	{
		initial_angle = init_angle;
	}

	void flush()
	{
		exist = false;
	}
};

Zombie* zombies;
sf::RectangleShape* zombie_rects;
int* zombies_hp;

sf::Sound zombie_hit;

void delay(float n_seconds);
void getDesktopResolution();

bool createNewBullet()
{
	for (int i = 0; i < max_num_bullets; i++)
	{
		if (bullets[i].exists() == false)
		{
			bullets[i].createBullet();
			return true;
		}
	}
	return false;
}

bool createNewZombie()
{
	int zombie_index;
	for (zombie_index = 0; zombie_index < max_num_zombies; zombie_index++)
	{
		if (zombies[zombie_index].exists() == false)
		{
			zombies[zombie_index].createZombie();
			zombies_hp[zombie_index] = 2;
			return true;
		}
	}
	return false;
}

void moveBullets()
{
	for (int i = 0; i < max_num_bullets; i++)
	{
		if (bullets[i].exists()) bullets[i].move();
	}
}

void moveZombies()
{
	for (int i = 0; i < max_num_zombies; i++)
	{
		if (zombies[i].exists())
		{
			if (zombies[i].inFocus())
			{
				if (!zombies[i].isAttacking())
				{
					zombies[i].move();
					zombie_rects[i].setTexture(&zombie_move[zombies[i].getCycle()]);
					zombie_rects[i].setPosition(zombies[i].getX(), zombies[i].getY());

					if (zombies[i].getCycle() == 1 && (zombies[i].getDesiredAngle() != zombies[i].getInitialAngle()))
					{
						zombie_rects[i].rotate(zombies[i].getDesiredAngle() - zombies[i].getInitialAngle());
						zombies[i].setInitialAngle(zombies[i].getDesiredAngle());
					}
				}
				else
				{
					zombies[i].move();
					zombie_rects[i].setTexture(&zombie_attack[zombies[i].getCycle()]);
				}
			}
			else zombies[i].move();
		}
	}
}

void checkBulletHits()
{
	for (int i = 0; i < max_num_zombies; i++)
	{
		if (zombies[i].exists() && zombies[i].inFocus())
		{
			for (int j = 0; j < max_num_bullets; j++)
			{
				if (bullets[j].exists())
				{
					if (abs(bullets[j].getX() - zombies[i].getX()) < 35 && abs(bullets[j].getY() - zombies[i].getY()) < 35)
					{
						bullets[j].destroy();
						zombies[i].shot();
					}
				}
			}
		}
	}
}

void flushZombies(bool full_flush)
{
	if (full_flush)
	{
		for (int i = 0; i < max_num_zombies; i++) zombies[i].flush();
	}
	else
	{
		int num_zombies_exist = 0;
		for (int i = 0; i < max_num_zombies; i++)
		{
			if (zombies[i].exists()) num_zombies_exist++;
		}

		if (num_zombies_exist > 3 * max_num_zombies / 4)
		{
			printf("Flushing zombies\n");
			for (int i = 0; i < max_num_zombies; i++)
			{
				if (!zombies[i].inFocus()) zombies[i].flush();
			}
		}
	}
}

void movePlayer()
{
	if (up_key)
	{
		if (abs(local_y - vertical / 2) > player_speed && local_y > 115) local_y = local_y - player_speed;
		else if (back_y <= 0)
		{
			back_y = back_y + player_speed;
			for (int i = 0; i < max_num_zombies; i++) if (zombies[i].exists()) zombies[i].move(0, player_speed);
			if (first_aid_spawned) aid_y = aid_y + player_speed;
		}
		else if(local_y > 115) local_y = local_y - player_speed;

		if (!aim_while_moving)
		{
			desired_angle = 0;
			targeting = false;
		}
		move_cycle++;
	}
	if (down_key)
	{
		if (abs(local_y - vertical / 2) > player_speed && local_y < vertical - 75) local_y = local_y + player_speed;
		else if (-back_y + vertical <= back_wid)
		{
			back_y = back_y - player_speed;
			for (int i = 0; i < max_num_zombies; i++) if (zombies[i].exists()) zombies[i].move(0, -player_speed);
			if (first_aid_spawned) aid_y = aid_y - player_speed;
		}
		else if (local_y < vertical - 75) local_y = local_y + player_speed;

		if (!aim_while_moving)
		{
			desired_angle = 180;
			targeting = false;
		}
		move_cycle++;
	}
	if (left_key)
	{
		if (abs(local_x - horizontal / 2) > player_speed && local_x > 75) local_x = local_x - player_speed;
		else if (back_x <= 0)
		{
			back_x = back_x + player_speed;
			for (int i = 0; i < max_num_zombies; i++) if (zombies[i].exists()) zombies[i].move(player_speed, 0);
			if (first_aid_spawned) aid_x = aid_x + player_speed;
		}
		else if (local_x > 75) local_x = local_x - player_speed;
		
		if (!aim_while_moving)
		{
			desired_angle = -90;
			targeting = false;
		}
		move_cycle++;
	}
	if (right_key)
	{
		if (abs(local_x - horizontal / 2) > player_speed && local_x < vertical - 75) local_x = local_x + player_speed;
		else if (-back_x + horizontal <= back_len)
		{
			back_x = back_x - player_speed;
			for (int i = 0; i < max_num_zombies; i++) if (zombies[i].exists()) zombies[i].move(-player_speed, 0);
			if (first_aid_spawned) aid_x = aid_x - player_speed;
		}
		else if(local_x < horizontal - 75) local_x = local_x + player_speed;

		if (!aim_while_moving)
		{
			desired_angle = 90;
			targeting = false;
		}
		move_cycle++;
	}

	if (!aim_while_moving)
	{
		if (up_key && left_key) desired_angle = -45;
		else if (up_key && right_key) desired_angle = 45;
		else if (down_key && left_key) desired_angle = -135;
		else if (down_key && right_key) desired_angle = 135;
	}

	if (move_cycle > 9) move_cycle = 0;
}

bool checkWave()
{
	if (score >= target)
	{
		wave++;
		flushZombies(true);

		if (wave == 2)
		{
			target = 50;
			zombie_spawn_cycle = 2;
			zombie_spawn_stage = 0;
		}
		else if (wave == 3)
		{
			target = 90;
			zombie_spawn_cycle = 1;
			zombie_spawn_stage = fps / 3;
		}
		else if (wave == 4)
		{
			target = 140;
			zombie_spawn_cycle = 1;
			zombie_spawn_stage = fps / 6;
		}
		else if (wave == 5)
		{
			target = 200;
			zombie_spawn_cycle = 1;
			zombie_spawn_stage = fps / 8;
		}
		else if (wave == 6)
		{
			target = 250;
			zombie_spawn_cycle = 1;
			zombie_spawn_stage = 0;
		}
		else
		{
			target = target + 100;
		}
		return true;
	}
	return false;
}

sf::Sound first_aid_pickup;

bool checkAid()
{
	if (abs(aid_x - local_x) < 45 && abs(aid_y - local_y) < 45)
	{
		if (player_health == 100) return false;
		else
		{
			first_aid_spawned = false;
			if (player_health < 80) 
			{
				player_health = player_health + 20;
				first_aid_pickup.play();
			}
			else 
			{
				player_health = 100;
				first_aid_pickup.play();
			}
		}
	}
	if (aid_x < 0 || aid_x > horizontal || aid_y < 0 || aid_y > vertical) first_aid_spawned = false;
}

void calibratePlayer()
{
	float base = local_y - target_y;
	float perp = target_x - local_x;

	if (base != 0) desired_angle = atan(float(perp / base)) * 180 / 3.14;
	if (base < 0) desired_angle = desired_angle + 180;
}

void gameLoop()
{
	sf::Texture floor_texture;
	floor_texture.loadFromFile("zombie_data/floor_entire.png");

	sf::RectangleShape floor(sf::Vector2f(back_len, back_wid));
	floor.setTexture(&floor_texture);
	floor.setPosition(back_x, back_y);

	sf::Image* shooter_images = new sf::Image[10];
	sf::Texture* shooter_textures = new sf::Texture[10];
	for (int i = 0; i < 10; i++)
	{
		shooter_images[i].loadFromFile("zombie_data/shooter_" + std::to_string(i) + ".png");
		shooter_images[i].createMaskFromColor(sf::Color::White);
		shooter_textures[i].loadFromImage(shooter_images[i]);
	}

	sf::RectangleShape shooter(sf::Vector2f(100, 150));
	shooter.setOrigin(73, 100);
	shooter.setTexture(&shooter_textures[0]);

	sf::Image* zombie_move_images = new sf::Image[9];
	sf::Image* zombie_attack_images = new sf::Image[9];
	zombie_move = new sf::Texture[9];
	zombie_attack = new sf::Texture[9];
	for (int i = 0; i < 9; i++)
	{
		zombie_move_images[i].loadFromFile("zombie_data/zombie_move_" + std::to_string(i) + ".png");
		zombie_move_images[i].createMaskFromColor(sf::Color::White);
		zombie_move[i].loadFromImage(zombie_move_images[i]);

		zombie_attack_images[i].loadFromFile("zombie_data/zombie_attack_" + std::to_string(i) + ".png");
		zombie_attack_images[i].createMaskFromColor(sf::Color::White);
		zombie_attack[i].loadFromImage(zombie_attack_images[i]);
	}

	zombie_rects = new sf::RectangleShape[max_num_zombies];
	for (int i = 0; i < max_num_zombies; i++)
	{
		zombie_rects[i].setSize(sf::Vector2f(150, 150));
		zombie_rects[i].setOrigin(75, 75);
	}

	bullet_circles = new sf::CircleShape[max_num_bullets];
	for (int i = 0; i < max_num_bullets; i++)
	{
		bullet_circles[i].setRadius(4);
		bullet_circles[i].setOrigin(4, 4);
		bullet_circles[i].setFillColor(sf::Color::Black);
	}

	sf::Image first_aid_image;
	first_aid_image.loadFromFile("zombie_data/first_aid.png");
	first_aid_image.createMaskFromColor(sf::Color::White);

	sf::Texture first_aid_texture;
	first_aid_texture.loadFromImage(first_aid_image);

	sf::RectangleShape first_aid_rect(sf::Vector2f(90,90));
	first_aid_rect.setTexture(&first_aid_texture);
	first_aid_rect.setOrigin(45, 45);

	sf::RectangleShape scoreboard(sf::Vector2f(horizontal + 10, 50));
	scoreboard.setPosition(-5, -5);
	scoreboard.setFillColor(sf::Color::Black);
	scoreboard.setOutlineThickness(5);
	scoreboard.setOutlineColor(sf::Color::White);

	sf::RectangleShape blue_screen(sf::Vector2f(horizontal, vertical));
	blue_screen.setPosition(0, 0);
	blue_screen.setFillColor(sf::Color::Blue);

	sf::Font font;
	font.loadFromFile("zombie_data/font/futura.ttf");

	sf::Text wave_info;
	wave_info.setFont(font);
	wave_info.setCharacterSize(30);
	wave_info.setPosition(horizontal / 2 - 150, 0);
	wave_info.setFillColor(sf::Color::White);
	wave_info.setStyle(sf::Text::Bold);

	sf::Text health_info;
	health_info.setFont(font);
	health_info.setCharacterSize(30);
	health_info.setPosition(horizontal - 160, 0);
	health_info.setFillColor(sf::Color::White);
	health_info.setStyle(sf::Text::Bold);

	sf::Text score_info;
	score_info.setFont(font);
	score_info.setCharacterSize(30);
	score_info.setPosition(0, 0);
	score_info.setFillColor(sf::Color::White);
	score_info.setStyle(sf::Text::Bold);

	sf::Text message;
	message.setFont(font);
	message.setCharacterSize(30);
	message.setPosition(0, 50);
	message.setFillColor(sf::Color::Black);
	message.setStyle(sf::Text::Bold);

	sf::SoundBuffer gunshot_buf;
	gunshot_buf.loadFromFile("zombie_data/sounds/gunshot.wav");

	sf::SoundBuffer player_hurt_buf;
	if (player_hurt_buf.loadFromFile("zombie_data/sounds/player_hurt.wav") == false) printf("Player hit.wav wasn't loaded");
	player_hurt.setBuffer(player_hurt_buf);

	sf::SoundBuffer zombie_attack_soundbuf;
	zombie_attack_soundbuf.loadFromFile("zombie_data/sounds/zombie_attack.wav");
	zombie_attack_sound.setBuffer(zombie_attack_soundbuf);
	zombie_attack_sound.setPlayingOffset(sf::milliseconds(900));

	sf::SoundBuffer zombie_die_buf;
	zombie_die_buf.loadFromFile("zombie_data/sounds/zombie_die.wav");
	zombie_die.setBuffer(zombie_die_buf);

	sf::SoundBuffer first_aid_spawned_buf;
	first_aid_spawned_buf.loadFromFile("zombie_data/sounds/first_aid_sound.ogg");
	first_aid_sound.setBuffer(first_aid_spawned_buf);

	sf::SoundBuffer first_aid_pickup_buf;
	first_aid_pickup_buf.loadFromFile("zombie_data/sounds/first_aid_pickup.ogg");
	first_aid_pickup.setBuffer(first_aid_pickup_buf);

	sf::SoundBuffer next_level_buf;
	next_level_buf.loadFromFile("zombie_data/sounds/next_level.wav");

	sf::Sound gunshot;
	gunshot.setBuffer(gunshot_buf);

	sf::Sound next_level;
	next_level.setBuffer(next_level_buf);

	sf::Music background;
	background.openFromFile("zombie_data/sounds/background.wav");
	background.play();
	background.setLoop(true);

	sf::RenderWindow window(sf::VideoMode(horizontal, vertical), "Zombie Apocalypse", sf::Style::Fullscreen);
	window.requestFocus();
	window.setFramerateLimit(fps);
	
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
				if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
				{
					up_key = true;
				}
				if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
				{
					down_key = true;
				}
				if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
				{
					left_key = true;
				}
				if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
				{
					right_key = true;
				}
			}
			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
				{
					up_key = false;
				}
				if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
				{
					down_key = false;
				}
				if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
				{
					left_key = false;
				}
				if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
				{
					right_key = false;
				}
			}
			if (event.type == sf::Event::MouseMoved)
			{
				target_x = sf::Mouse::getPosition(window).x;
				target_y = sf::Mouse::getPosition(window).y;
				targeting = true;

				calibratePlayer();
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				target_x = sf::Mouse::getPosition(window).x;
				target_y = sf::Mouse::getPosition(window).y;
				if (!up_key && !down_key && !left_key && !right_key)
				{
					targeting = true;
					calibratePlayer();
				}
				else
				{
					targeting = false;
					message.setString("You can't shoot in the right direction while moving");
				}

				if (event.mouseButton.button == sf::Mouse::Left)
				{
					mouse_left_pressed = true;
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					mouse_left_released = true;
				}
			}
		}
		window.clear();

		if (mouse_left_pressed && mouse_left_released && targeting)
		{
			if (createNewBullet()) gunshot.play();
			mouse_left_released = false;
		}

		if (game_cycle % zombie_spawn_cycle == 0 && cycle_stage == zombie_spawn_stage) createNewZombie();

		movePlayer();
		moveBullets();
		moveZombies();
		checkBulletHits();

		if (desired_angle != initial_angle)
		{
			shooter.rotate(desired_angle - initial_angle);
			initial_angle = desired_angle;
		}

		if (!player_health) window.close();

		floor.setPosition(back_x, back_y);
		shooter.setPosition(local_x, local_y);
		for (int i = 0; i < max_num_bullets; i++) if (bullets[i].exists()) bullet_circles[i].setPosition(bullets[i].getX(), bullets[i].getY());
		if (first_aid_spawned)
		{
			first_aid_rect.setPosition(aid_x, aid_y);
			if (!checkAid()) message.setString("Your health is full");
		}

		window.draw(floor);
		shooter.setTexture(&shooter_textures[move_cycle]);
		window.draw(shooter);
		for (int i = 0; i < max_num_bullets; i++) if (bullets[i].exists()) window.draw(bullet_circles[i]);
		for (int i = 0; i < max_num_zombies; i++) if (zombies[i].exists() && zombies[i].inFocus()) if(zombies[i].inFocus()) window.draw(zombie_rects[i]);
		if (first_aid_spawned) window.draw(first_aid_rect);

		window.draw(scoreboard);
		window.draw(score_info);
		window.draw(wave_info);
		window.draw(health_info);
		window.draw(message);

		cycle_stage++;
		if (cycle_stage == fps)
		{
			flushZombies(false);
			if (checkWave())
			{
				background.stop();
				next_level.play();
				message.setString("Next Wave starting in three seconds");

				window.draw(blue_screen);
				window.draw(message);
				window.display();
				delay(3);

				message.setString("");
				first_aid_spawned = false;
				background.play();
			}
			cycle_stage = 0;
			game_cycle++;

			score_info.setString("Kills: " + std::to_string(score));
			wave_info.setString("Wave " + std::to_string(wave) + "- Target: " + std::to_string(target));
			health_info.setString("Health: " + std::to_string(player_health));
		}

		window.display();
	}
}

void init()
{
	game_cycle = cycle_stage = 0;
	fps = 24;

	up_key = down_key = left_key = right_key = mouse_left_pressed = false;
	mouse_left_released = true;
	targeting = false;
	first_aid_spawned = false;

	back_len = 2400;
	back_wid = 2400;

	back_x = -back_len / 2 + horizontal / 2;
	back_y = -back_wid / 2 + vertical / 2;

	local_x = horizontal / 2;
	local_y = vertical / 2;
	move_cycle = 0;

	player_speed = 8;
	player_health = 100;
	score = 0;

	initial_angle = desired_angle = 0;
	aim_while_moving = false;

	max_num_bullets = 16;
	bullets = new Bullet[max_num_bullets];
	for (int i = 0; i < max_num_bullets; i++) bullets[i].destroy();
	bullet_speed = 20;

	max_num_zombies = 24;
	zombies = new Zombie[max_num_zombies];
	zombie_speed = 4;

	zombies_hp = new int[max_num_zombies];
	for (int i = 0; i < max_num_zombies; i++) zombies_hp[i] = 0;

	wave = 1;
	target = 20;
	zombie_spawn_cycle = 2;
	zombie_spawn_stage = fps / 2;
}

int main()
{
	srand(time(NULL));

	printf("---> Zombie Apocalypse <---\n\n");

	printf("INSTRUCTIONS\n");
	printf("Fight off successive waves of zombies!\n");
	printf("Use WASD/Arrows keys to move your character and the mouse to aim and shoot at zombies\n");
	printf("If zombies get close enough to you, they'll attack you\n");
	printf("The game ends when your health drops to 0\n\n");
	printf("Some zombies drop first-aid which can be used only if you're wounded\n");
	printf("Any spawned first aid will despawn if it goes out of focus\n");
	printf("You can't shoot while moving\n\n");

	printf("BUGS\n");
	printf("Stray texture renduring while zombie spawn can be seen. Can't be eliminated completely\n");
	printf("Sometimes, the player can shoot while moving, while aiming in an opposite direction\n");
	printf("If the player attempts to shoot while moving, stray shots are fired after the movement stops\n");
	printf("The player_hurt.wav sound is played late sometimes\n\n");

	printf("ABOUT\n");
	printf("Game developed by ChanRT\n");
	printf("Sounds obtained from freesound.org and art from opengameart.org\n\n");

	printf("PRESS ANY KEY TO START GAME");
	_getch();
	printf("\nLoading, please wait");

	getDesktopResolution();
	init();
	gameLoop();
}

void getDesktopResolution()
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

void delay(float n_seconds)
{
	int milli = 1000 * n_seconds;
	clock_t start = clock();
	while (clock() < start + milli);
}