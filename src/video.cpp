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
			case sf::Event::MouseButtonPressed:
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					if (current_screen == MAIN_MENU) {
						if (check_button_clicked(play_rect)) {
							current_screen = MODE_SELECTION;
							mode_selected = CLASSIC; // default mode
							modes[CLASSIC]->button_text.setFillColor(sf::Color::Green);
						} else if (check_button_clicked(options_rect)) {
							// current_screen = OPTIONS;
						} else if (check_button_clicked(quit_rect)) {
							window.close();
						}
					} else if (current_screen == MODE_SELECTION) {
						for (auto &c : modes) {
							if (check_button_clicked(c.second->button_rect)) {
								// deselect all other buttons
								for (auto &c : modes) {
									c.second->button_text.setFillColor(sf::Color::Black);
								}
								c.second->button_text.setFillColor(sf::Color::Green);

								mode_selected = c.first;
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
						if (check_button_clicked(ok_button.button_rect)) {
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
		} else if (current_screen == MODE_SELECTION) {
			draw_mode_selection();
		} else if (current_screen == SESSION) {
			draw_session();
		} else if (current_screen == SUMMARY) {
			draw_summary();
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

	window.draw(title_text);
	window.draw(play_text);
	window.draw(options_text);
	window.draw(quit_text);
}

void Video::draw_mode_selection()
{
	window.draw(modes[CLASSIC]->button_rect);
	window.draw(modes[PRECISION]->button_rect);
	window.draw(modes[SPEED]->button_rect);
	window.draw(begin_rect);
	window.draw(back_rect);

	window.draw(modes[CLASSIC]->button_text);
	window.draw(modes[PRECISION]->button_text);
	window.draw(modes[SPEED]->button_text);
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
	window.draw(accuracy_text);
	window.draw(timer_text);
}

void Video::draw_summary()
{
	window.draw(accuracy_summary_text);
	window.draw(ok_button.button_rect);
	window.draw(ok_button.button_text);
}

void Video::set_up_main_menu_elements()
{
	play_rect.setSize(sf::Vector2f(200, 50));
	play_rect.setPosition(sf::Vector2f(0, 100));

	options_rect.setSize(sf::Vector2f(200, 50));
	options_rect.setPosition(sf::Vector2f(0, 175));

	quit_rect.setSize(sf::Vector2f(200, 50));
	quit_rect.setPosition(sf::Vector2f(0, 250));

	title_text.setFont(font);
	title_text.setString("AimTrainer");
	title_text.setFillColor(sf::Color::Green);
	title_text.setCharacterSize(40);
	title_text.setPosition(sf::Vector2f(10, 10));

	play_text.setFont(font);
	play_text.setString("Play");
	play_text.setFillColor(sf::Color::Black);
	play_text.setCharacterSize(24);
	play_text.setPosition(sf::Vector2f(play_rect.getPosition().x, play_rect.getPosition().y));

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
	modes[CLASSIC]->button_rect.setSize(sf::Vector2f(200, 100));
	modes[CLASSIC]->button_rect.setPosition(sf::Vector2f(0, 100));
	modes[CLASSIC]->button_text.setFont(font);
	modes[CLASSIC]->button_text.setString("Classic");
	modes[CLASSIC]->button_text.setFillColor(sf::Color::Black);
	modes[CLASSIC]->button_text.setCharacterSize(24);
	modes[CLASSIC]->button_text.setPosition(sf::Vector2f(modes[CLASSIC]->button_rect.getPosition().x,
														 modes[CLASSIC]->button_rect.getPosition().y));

	// precision
	modes[PRECISION]->button_rect.setSize(sf::Vector2f(200, 100));
	modes[PRECISION]->button_rect.setPosition(sf::Vector2f(300, 100));
	modes[PRECISION]->button_text.setFont(font);
	modes[PRECISION]->button_text.setString("Precision");
	modes[PRECISION]->button_text.setFillColor(sf::Color::Black);
	modes[PRECISION]->button_text.setCharacterSize(24);
	modes[PRECISION]->button_text.setPosition(sf::Vector2f(modes[PRECISION]->button_rect.getPosition().x,
														   modes[PRECISION]->button_rect.getPosition().y));

	// speed
	modes[SPEED]->button_rect.setSize(sf::Vector2f(200, 100));
	modes[SPEED]->button_rect.setPosition(sf::Vector2f(600, 100));
	modes[SPEED]->button_text.setFont(font);
	modes[SPEED]->button_text.setString("Speed");
	modes[SPEED]->button_text.setFillColor(sf::Color::Black);
	modes[SPEED]->button_text.setCharacterSize(24);
	modes[SPEED]->button_text.setPosition(sf::Vector2f(modes[SPEED]->button_rect.getPosition().x,
													   modes[SPEED]->button_rect.getPosition().y));

	begin_rect.setSize(sf::Vector2f(200, 50));
	begin_rect.setPosition(sf::Vector2f(0, window.getSize().y - begin_rect.getSize().y));

	back_rect.setSize(sf::Vector2f(200, 50));
	back_rect.setPosition(sf::Vector2f(window.getSize().x - back_rect.getSize().x,
									   window.getSize().y - back_rect.getSize().y));

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

	ok_button.button_rect.setSize(sf::Vector2f(200, 50));
	ok_button.button_rect.setPosition(
									  sf::Vector2f(0, window.getSize().y - ok_button.button_rect.getSize().y));

	ok_button.button_text.setFont(font);
	ok_button.button_text.setString("OK");
	ok_button.button_text.setCharacterSize(24);
	ok_button.button_text.setFillColor(sf::Color::Black);
	ok_button.button_text.setPosition(
									  sf::Vector2f(ok_button.button_rect.getPosition().x, ok_button.button_rect.getPosition().y));
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
