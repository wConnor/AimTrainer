#include "video.hpp"

Video::Video()
{
	std::srand(std::time(nullptr));

	modes.push_back(std::make_pair(std::make_unique<sf::RectangleShape>(), CLASSIC));
	modes.push_back(std::make_pair(std::make_unique<sf::RectangleShape>(), PRECISION));
	modes.push_back(std::make_pair(std::make_unique<sf::RectangleShape>(), SPEED));
}

bool Video::init()
{
	window.create(sf::VideoMode(800, 600), "AimTrainer");
	window.setVerticalSyncEnabled(true);

	if (!font.loadFromFile("./assets/fonts/Roboto-Regular.ttf")) {
		return 1;
	}

	set_up_main_menu_elements();
	set_up_mode_selection_elements();
	set_up_session_elements();

	// main loop
	while (window.isOpen()) {
		sf::Event e;

		// event loop
		while (window.pollEvent(e)) {
			switch (e.type) {
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::MouseButtonPressed:
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					if (current_screen == MAIN_MENU) {
						if (check_button_clicked(play_rect)) {
							current_screen = MODE_SELECTION;
						} else if (check_button_clicked(options_rect)) {
							current_screen = OPTIONS;
						} else if (check_button_clicked(quit_rect)) {
							window.close();
						}
					} else if (current_screen == MODE_SELECTION) {
						for (auto &c : modes) {
							if (check_button_clicked(*c.first)) {
								// deselect all other buttons
								for (auto &c : modes) {
									c.first->setFillColor(sf::Color::White);
								}
								c.first->setFillColor(sf::Color::Green);

								mode_selected = c.second;
								break;
							}
						}
						if (check_button_clicked(begin_rect)) {
							prepare_session();
						} else if (check_button_clicked(back_rect)) {
							current_screen = MAIN_MENU;
						}
					} else if (current_screen == SESSION) {
						if (targets_hit == max_targets - 1) {
							targets_hit++;
							current_screen = MAIN_MENU; // will be changed to stats screen
							targets_hit = 0;
							targets_hit_text.setString("Targets hit: " + std::to_string(targets_hit));
						} else if (check_button_clicked(*targets[targets_hit])) {
							targets_hit++;
							targets_hit_text.setString("Targets hit: " + std::to_string(targets_hit));
						}
					}
				}

			default:
				break;
			}
		}
		window.clear(sf::Color::Black);

		if (current_screen == MAIN_MENU) {
			draw_main_menu();
		} else if (current_screen == MODE_SELECTION) {
			draw_mode_selection();
		} else if (current_screen == SESSION) {
			draw_session();
		}

		window.display();
	}
	return 0;
}

bool Video::set_resolution(const unsigned int &width, const unsigned int &height)
{
	window.setSize(sf::Vector2u(width, height));
	return 0;
}

void Video::draw_main_menu()
{
	window.draw(play_rect);
	window.draw(options_rect);
	window.draw(quit_rect);

	window.draw(play_text);
	window.draw(options_text);
	window.draw(quit_text);
}

void Video::draw_mode_selection()
{
	window.draw(*modes[0].first);
	window.draw(*modes[1].first);
	window.draw(*modes[2].first);
	window.draw(begin_rect);
	window.draw(back_rect);

	window.draw(classic_text);
	window.draw(precision_text);
	window.draw(speed_text);
	window.draw(begin_text);
	window.draw(back_text);
}

void Video::draw_session()
{
	std::ostringstream timer_ss;
	timer_ss.precision(2);
	timer_ss << std::fixed << current_session_timer.getElapsedTime().asSeconds();

	timer_text.setString("Elapsed: " + timer_ss.str());
	window.draw(*targets[targets_hit]);
	window.draw(targets_hit_text);
	window.draw(timer_text);
}

void Video::set_up_main_menu_elements()
{
	play_rect.setSize(sf::Vector2f(200, 50));
	play_rect.setPosition(sf::Vector2f(0, 100));

	options_rect.setSize(sf::Vector2f(200, 50));
	options_rect.setPosition(sf::Vector2f(0, 200));

	quit_rect.setSize(sf::Vector2f(200, 50));
	quit_rect.setPosition(sf::Vector2f(0, 300));

	play_text.setFont(font);
	play_text.setString("Start");
	play_text.setFillColor(sf::Color::Black);
	play_text.setCharacterSize(24);
	play_text.setPosition(sf::Vector2f(0, 100));

	options_text.setFont(font);
	options_text.setString("Options");
	options_text.setFillColor(sf::Color::Black);
	options_text.setCharacterSize(24);
	options_text.setPosition(sf::Vector2f(options_rect.getPosition().x, options_rect.getPosition().y));

	quit_text.setFont(font);
	quit_text.setString("Quit");
	quit_text.setFillColor(sf::Color::Black);
	quit_text.setCharacterSize(24);
	quit_text.setPosition(sf::Vector2f(quit_rect.getPosition().x, quit_rect.getPosition().y));
}

void Video::set_up_mode_selection_elements()
{
	// classic
	modes[0].first->setSize(sf::Vector2f(200, 100));
	modes[0].first->setPosition(sf::Vector2f(0, 100));

	// precision
	modes[1].first->setSize(sf::Vector2f(200, 100));
	modes[1].first->setPosition(sf::Vector2f(300, 100));

	// speed
	modes[2].first->setSize(sf::Vector2f(200, 100));
	modes[2].first->setPosition(sf::Vector2f(600, 100));

	begin_rect.setSize(sf::Vector2f(200, 50));
	begin_rect.setPosition(sf::Vector2f(0, window.getSize().y - begin_rect.getSize().y));

	back_rect.setSize(sf::Vector2f(200, 50));
	back_rect.setPosition(sf::Vector2f(window.getSize().x - back_rect.getSize().x,
									   window.getSize().y - back_rect.getSize().y));

	classic_text.setFont(font);
	classic_text.setString("Classic");
	classic_text.setFillColor(sf::Color::Black);
	classic_text.setCharacterSize(24);
	classic_text.setPosition(sf::Vector2f(modes[0].first->getPosition().x, modes[0].first->getPosition().y));

	precision_text.setFont(font);
	precision_text.setString("Precision");
	precision_text.setFillColor(sf::Color::Black);
	precision_text.setCharacterSize(24);
	precision_text.setPosition(
							   sf::Vector2f(modes[1].first->getPosition().x, modes[1].first->getPosition().y));

	speed_text.setFont(font);
	speed_text.setString("Speed");
	speed_text.setFillColor(sf::Color::Black);
	speed_text.setCharacterSize(24);
	speed_text.setPosition(sf::Vector2f(modes[2].first->getPosition().x, modes[2].first->getPosition().y));

	begin_text.setFont(font);
	begin_text.setString("Begin");
	begin_text.setFillColor(sf::Color::Black);
	begin_text.setCharacterSize(24);
	begin_text.setPosition(sf::Vector2f(begin_rect.getPosition().x, begin_rect.getPosition().y));

	back_text.setFont(font);
	back_text.setString("Back");
	back_text.setFillColor(sf::Color::Black);
	back_text.setCharacterSize(24);
	back_text.setPosition(sf::Vector2f(back_rect.getPosition().x, back_rect.getPosition().y));
}

void Video::set_up_session_elements()
{
	targets_hit_text.setFont(font);
	targets_hit_text.setString("Targets hit: " + std::to_string(targets_hit));
	targets_hit_text.setFillColor(sf::Color::Cyan);
	targets_hit_text.setCharacterSize(24);
	targets_hit_text.setPosition(0, 0);

	timer_text.setFont(font);
	timer_text.setFillColor(sf::Color::Magenta);
	timer_text.setCharacterSize(24);
	timer_text.setPosition(0, 30);
}

bool Video::check_button_clicked(const sf::RectangleShape &rect)
{
	sf::FloatRect cursor_rect(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y,
							  CURSOR_WIDTH, CURSOR_HEIGHT);

	return cursor_rect.intersects(rect.getGlobalBounds());
}

bool Video::check_target_clicked(const sf::CircleShape &target)
{
	// TODO: implement circle and rectangle (cursor) collision.
	return true;
}

void Video::select_mode(const Mode &mode)
{
}

void Video::prepare_session()
{
	targets_hit = 0;

	switch (mode_selected) {
	case CLASSIC:
		randomise_targets(max_targets);
		break;

	case PRECISION:
		break;

	case SPEED:
		break;
	}

	current_screen = SESSION;
	current_session_timer.restart();
}

void Video::randomise_targets(const int &amount)
{
	targets.clear();

	// temporarily using rectangles until circle collision detection is implemented.
	for (int i = 0; i <= amount; ++i) {
		targets.push_back(std::make_unique<sf::RectangleShape>());
		targets[i]->setPosition(sf::Vector2f(rand() % window.getSize().x, rand() % window.getSize().y));
		targets[i]->setSize(sf::Vector2f(20, 20));
	}

	// for (int i = 0; i != amount; ++i) {
	//	targets.push_back(std::make_unique<sf::CircleShape>());
	//	targets[i]->setRadius(30);
	//	targets[i]->setPointCount(30);
	//	targets[i]->setPosition(sf::Vector2f(rand() % window.getSize().x,
	//										 rand() %
	// window.getSize().y));
	// }
}

Video::~Video()
{
}
