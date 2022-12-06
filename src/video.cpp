#include "video.hpp"

Video::Video()
{
	std::srand(std::time(nullptr));

	modes[CLASSIC] = std::make_unique<Button>();
	modes[PRECISION] = std::make_unique<Button>();
	modes[SPEED] = std::make_unique<Button>();
}

bool Video::init()
{
	window.create(sf::VideoMode(800, 600), "AimTrainer");
	window.setVerticalSyncEnabled(true);

	if (!font.loadFromFile("./assets/fonts/Roboto-Regular.ttf")) {
		return 1;
	}

	set_up_main_menu_elements();
	set_up_options_elements();
	set_up_mode_selection_elements();
	set_up_session_elements();
	set_up_summary_elements();

	// main loop
	while (window.isOpen()) {
		sf::Event e;

		// event loop
		while (window.pollEvent(e)) {
			switch (e.type) {
			case sf::Event::KeyPressed:
				if (e.key.code == sf::Keyboard::Escape && current_screen == SESSION) {
					end_session();
				}

				break;
			case sf::Event::Closed:
				window.close();

				break;

			case sf::Event::MouseMoved:
				if (custom_cursor) {
					cursor.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
				}

				break;
			case sf::Event::MouseButtonPressed:
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					if (current_screen == MAIN_MENU) {
						if (check_button_clicked(play_button.rect)) {
							current_screen = MODE_SELECTION;
							mode_selected = CLASSIC; // default mode
							modes[CLASSIC]->text.setFillColor(sf::Color::Green);
						} else if (check_button_clicked(options_button.rect)) {
							current_screen = OPTIONS;
						} else if (check_button_clicked(quit_button.rect)) {
							window.close();
						}

					} else if (current_screen == OPTIONS) {
						if (check_button_clicked(cursor_crosshair)) {
							window.setMouseCursorVisible(true);
							custom_cursor = false;
						}
						// to be changed to for-auto loop once more xhairs added.
						if (check_button_clicked(plus_crosshair)) {
							cursor.setTexture(plus_red);
							cursor.setPosition(sf::Mouse::getPosition(window).x,
											   sf::Mouse::getPosition(window).y);
							window.setMouseCursorVisible(false);
							custom_cursor = true;
						}
						if (check_button_clicked(ok_options_button.rect)) {
							current_screen = MAIN_MENU;
						}
					} else if (current_screen == MODE_SELECTION) {
						for (auto &c : modes) {
							if (check_button_clicked(c.second->rect)) {
								// deselect all other buttons
								for (auto &c : modes) {
									c.second->text.setFillColor(sf::Color::Black);
								}
								c.second->text.setFillColor(sf::Color::Green);

								mode_selected = c.first;
								break;
							}
						}
						if (check_button_clicked(begin_button.rect)) {
							prepare_session();
						} else if (check_button_clicked(back_button.rect)) {
							current_screen = MAIN_MENU;
						}
					} else if (current_screen == SESSION) {
						if (targets_hit == max_targets - 1) {
							end_session();
							current_screen = SUMMARY;
						}

						check_button_clicked(*targets[targets_hit]) ? targets_hit++ : targets_missed++;
						targets_hit_text.setString("Targets hit: " + std::to_string(targets_hit));
						std::ostringstream acc_ss;
						acc_ss.precision(2);
						acc_ss << std::fixed
							   << static_cast<double>(targets_hit) /
							(static_cast<double>(targets_hit) +
							 static_cast<double>(targets_missed)) *
							100.0;
						accuracy_text.setString("Accuracy: " + acc_ss.str() + "%");
					} else if (current_screen == SUMMARY) {
						if (check_button_clicked(ok_summary_button.rect)) {
							current_screen = MAIN_MENU;
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
		} else if (current_screen == OPTIONS) {
			draw_options();
		} else if (current_screen == MODE_SELECTION) {
			draw_mode_selection();
		} else if (current_screen == SESSION) {
			draw_session();
		} else if (current_screen == SUMMARY) {
			draw_summary();
		}

		if (custom_cursor) {
			window.draw(cursor);
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
	window.draw(play_button.rect);
	window.draw(options_button.rect);
	window.draw(quit_button.rect);

	window.draw(title_text);
	window.draw(play_button.text);
	window.draw(options_button.text);
	window.draw(quit_button.text);
}

void Video::draw_options()
{
	window.draw(options_text);
	window.draw(crosshair_text);

	window.draw(ok_options_button.rect);
	window.draw(ok_options_button.text);

	window.draw(cursor_crosshair);
	window.draw(plus_crosshair);
}

void Video::draw_mode_selection()
{
	window.draw(modes[CLASSIC]->rect);
	window.draw(modes[PRECISION]->rect);
	window.draw(modes[SPEED]->rect);
	window.draw(begin_button.rect);
	window.draw(back_button.rect);

	window.draw(selection_text);
	window.draw(modes[CLASSIC]->text);
	window.draw(modes[PRECISION]->text);
	window.draw(modes[SPEED]->text);
	window.draw(begin_button.text);
	window.draw(back_button.text);
}

void Video::draw_session()
{
	std::ostringstream timer_ss;
	timer_ss.precision(2);
	timer_ss << std::fixed << current_session_timer.getElapsedTime().asSeconds();

	timer_text.setString("Elapsed: " + timer_ss.str());
	window.draw(*targets[targets_hit]);
	window.draw(targets_hit_text);
	window.draw(accuracy_text);
	window.draw(timer_text);
}

void Video::draw_summary()
{
	window.draw(accuracy_summary_text);
	window.draw(ok_summary_button.rect);
	window.draw(ok_summary_button.text);
}

void Video::set_up_main_menu_elements()
{
	play_button.rect.setSize(sf::Vector2f(200, 50));
	play_button.rect.setPosition(sf::Vector2f(0, 100));

	options_button.rect.setSize(sf::Vector2f(200, 50));
	options_button.rect.setPosition(sf::Vector2f(0, 175));

	quit_button.rect.setSize(sf::Vector2f(200, 50));
	quit_button.rect.setPosition(sf::Vector2f(0, 250));

	title_text.setFont(font);
	title_text.setString("AimTrainer");
	title_text.setFillColor(sf::Color::Green);
	title_text.setStyle(sf::Text::Bold);
	title_text.setCharacterSize(40);
	title_text.setPosition(sf::Vector2f(10, 10));

	play_button.text.setFont(font);
	play_button.text.setString("Play");
	play_button.text.setFillColor(sf::Color::Black);
	play_button.text.setCharacterSize(24);
	play_button.text.setPosition(
								 sf::Vector2f(play_button.rect.getPosition().x, play_button.rect.getPosition().y));

	options_button.text.setFont(font);
	options_button.text.setString("Options");
	options_button.text.setFillColor(sf::Color::Black);
	options_button.text.setCharacterSize(24);
	options_button.text.setPosition(
									sf::Vector2f(options_button.rect.getPosition().x, options_button.rect.getPosition().y));

	quit_button.text.setFont(font);
	quit_button.text.setString("Quit");
	quit_button.text.setFillColor(sf::Color::Black);
	quit_button.text.setCharacterSize(24);
	quit_button.text.setPosition(
								 sf::Vector2f(quit_button.rect.getPosition().x, quit_button.rect.getPosition().y));
}

void Video::set_up_options_elements()
{
	options_text.setFont(font);
	options_text.setCharacterSize(40);
	options_text.setString("Options");
	options_text.setStyle(sf::Text::Bold);
	options_text.setPosition(10, 10);
	options_text.setFillColor(sf::Color::White);

	crosshair_text.setFont(font);
	crosshair_text.setCharacterSize(24);
	crosshair_text.setString("Crosshair:");
	crosshair_text.setPosition(10, 100);
	crosshair_text.setFillColor(sf::Color::White);

	if (!default_cursor.loadFromFile("./assets/xhair/default.png")) {
		// error.
	}
	cursor_crosshair.setTexture(default_cursor);
	cursor_crosshair.setPosition(sf::Vector2f(150, 107));

	if (!plus_red.loadFromFile("./assets/xhair/plus_red.png")) {
		// error.
	}
	plus_crosshair.setTexture(plus_red);
	plus_crosshair.setPosition(200, 115);

	ok_options_button.rect.setSize(sf::Vector2f(200, 50));
	ok_options_button.rect.setPosition(
									   sf::Vector2f(0, window.getSize().y - ok_options_button.rect.getSize().y));

	ok_options_button.text.setFont(font);
	ok_options_button.text.setString("OK");
	ok_options_button.text.setCharacterSize(24);
	ok_options_button.text.setFillColor(sf::Color::Black);
	ok_options_button.text.setPosition(
									   sf::Vector2f(ok_options_button.rect.getPosition().x, ok_options_button.rect.getPosition().y));
}

void Video::set_up_mode_selection_elements()
{
	selection_text.setFont(font);
	selection_text.setCharacterSize(40);
	selection_text.setString("Select Mode");
	selection_text.setStyle(sf::Text::Bold);
	selection_text.setPosition(10, 10);
	selection_text.setFillColor(sf::Color::White);

	// classic
	modes[CLASSIC]->rect.setSize(sf::Vector2f(200, 100));
	modes[CLASSIC]->rect.setPosition(sf::Vector2f(0, 100));
	modes[CLASSIC]->text.setFont(font);
	modes[CLASSIC]->text.setString("Classic");
	modes[CLASSIC]->text.setFillColor(sf::Color::Black);
	modes[CLASSIC]->text.setCharacterSize(24);
	modes[CLASSIC]->text.setPosition(
									 sf::Vector2f(modes[CLASSIC]->rect.getPosition().x, modes[CLASSIC]->rect.getPosition().y));

	// precision
	modes[PRECISION]->rect.setSize(sf::Vector2f(200, 100));
	modes[PRECISION]->rect.setPosition(sf::Vector2f(300, 100));
	modes[PRECISION]->text.setFont(font);
	modes[PRECISION]->text.setString("Precision\n (not implemented)");
	modes[PRECISION]->text.setFillColor(sf::Color::Black);
	modes[PRECISION]->text.setCharacterSize(24);
	modes[PRECISION]->text.setPosition(
									   sf::Vector2f(modes[PRECISION]->rect.getPosition().x, modes[PRECISION]->rect.getPosition().y));

	// speed
	modes[SPEED]->rect.setSize(sf::Vector2f(200, 100));
	modes[SPEED]->rect.setPosition(sf::Vector2f(600, 100));
	modes[SPEED]->text.setFont(font);
	modes[SPEED]->text.setString("Speed\n (not implemented)");
	modes[SPEED]->text.setFillColor(sf::Color::Black);
	modes[SPEED]->text.setCharacterSize(24);
	modes[SPEED]->text.setPosition(
								   sf::Vector2f(modes[SPEED]->rect.getPosition().x, modes[SPEED]->rect.getPosition().y));

	begin_button.rect.setSize(sf::Vector2f(200, 50));
	begin_button.rect.setPosition(sf::Vector2f(0, window.getSize().y - begin_button.rect.getSize().y));

	back_button.rect.setSize(sf::Vector2f(200, 50));
	back_button.rect.setPosition(sf::Vector2f(window.getSize().x - back_button.rect.getSize().x,
											  window.getSize().y - back_button.rect.getSize().y));

	begin_button.text.setFont(font);
	begin_button.text.setString("Begin");
	begin_button.text.setFillColor(sf::Color::Black);
	begin_button.text.setCharacterSize(24);
	begin_button.text.setPosition(
								  sf::Vector2f(begin_button.rect.getPosition().x, begin_button.rect.getPosition().y));

	back_button.text.setFont(font);
	back_button.text.setString("Back");
	back_button.text.setFillColor(sf::Color::Black);
	back_button.text.setCharacterSize(24);
	back_button.text.setPosition(
								 sf::Vector2f(back_button.rect.getPosition().x, back_button.rect.getPosition().y));
}

void Video::set_up_session_elements()
{
	targets_hit_text.setFont(font);
	targets_hit_text.setString("Targets hit: " + std::to_string(targets_hit));
	targets_hit_text.setFillColor(sf::Color::Cyan);
	targets_hit_text.setCharacterSize(24);
	targets_hit_text.setPosition(0, 0);

	accuracy_text.setFont(font);
	accuracy_text.setString("Accuracy: 100.00%");
	accuracy_text.setFillColor(sf::Color::Magenta);
	accuracy_text.setCharacterSize(24);
	accuracy_text.setPosition(0, 30);

	timer_text.setFont(font);
	timer_text.setFillColor(sf::Color::Magenta);
	timer_text.setCharacterSize(24);
	timer_text.setPosition(0, 60);
}

void Video::set_up_summary_elements()
{
	accuracy_summary_text.setFont(font);
	accuracy_summary_text.setCharacterSize(24);
	accuracy_summary_text.setFillColor(sf::Color::Cyan);
	accuracy_summary_text.setPosition(10, 10);

	ok_summary_button.rect.setSize(sf::Vector2f(200, 50));
	ok_summary_button.rect.setPosition(
									   sf::Vector2f(0, window.getSize().y - ok_summary_button.rect.getSize().y));

	ok_summary_button.text.setFont(font);
	ok_summary_button.text.setString("OK");
	ok_summary_button.text.setCharacterSize(24);
	ok_summary_button.text.setFillColor(sf::Color::Black);
	ok_summary_button.text.setPosition(
									   sf::Vector2f(ok_summary_button.rect.getPosition().x, ok_summary_button.rect.getPosition().y));
}

bool Video::check_button_clicked(const sf::Sprite &sprite)
{
	sf::FloatRect cursor_rect(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y,
							  CURSOR_WIDTH, CURSOR_HEIGHT);

	return cursor_rect.intersects(sprite.getGlobalBounds());
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
	return 0;
}

void Video::prepare_session()
{
	targets_hit = 0;
	targets_missed = 0;

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

void Video::end_session()
{
	targets_hit++;
	current_screen = MAIN_MENU; // will be changed to stats screen

	std::ostringstream acc_ss;
	acc_ss.precision(2);
	acc_ss << std::fixed
		   << static_cast<double>(targets_hit) /
		(static_cast<double>(targets_hit) + static_cast<double>(targets_missed)) * 100.0;
	accuracy_summary_text.setString("Accuracy: " + acc_ss.str() + "%");

	targets_hit = 0;
	targets_missed = 0;
	targets_hit_text.setString("Targets hit: " + std::to_string(targets_hit));
	accuracy_text.setString("Accuracy: 100.00%");
}

void Video::randomise_targets(const int &amount)
{
	targets.clear();

	// temporarily using rectangles until circle collision detection is implemented.
	for (int i = 0; i <= amount; ++i) {
		targets.push_back(std::make_unique<sf::RectangleShape>());
		targets[i]->setPosition(sf::Vector2f(rand() % window.getSize().x, rand() % window.getSize().y));
		targets[i]->setSize(sf::Vector2f(40, 40));
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
