#include "video.hpp"

Video::Video()
{
	std::srand(std::time(nullptr));

	modes[CLASSIC] = std::make_unique<Button>();
	modes[PRECISION] = std::make_unique<Button>();
	modes[SPEED] = std::make_unique<Button>();

	input_focus["target_count"] = {std::make_unique<Button>(), false};
	input_focus["on_screen_count"] = {std::make_unique<Button>(), false};
	input_focus["time"] = {std::make_unique<Button>(), false};
}

bool Video::init()
{
	window.create(sf::VideoMode(800, 600), "AimTrainer", sf::Style::Titlebar | sf::Style::Close);
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
					current_screen = MAIN_MENU;
				}

				if (e.key.code == sf::Keyboard::BackSpace && current_screen == MODE_SELECTION) {
					for (auto &c : input_focus) {
						if (c.second.second && !c.second.first->text.getString().isEmpty()) {
							std::string temp = c.second.first->text.getString();
							temp.pop_back();
							c.second.first->text.setString(temp);
							break;
						}
					}
				}

				break;
			case sf::Event::TextEntered:
				if (current_screen == MODE_SELECTION) {
					if (e.text.unicode < 128) {
						char casted = static_cast<char>(e.text.unicode);
						if (casted >= '0' && casted <= '9') {
							for (auto &c : input_focus) {
								if (c.second.second) {
									std::string temp = c.second.first->text.getString();
									temp += casted;
									c.second.first->text.setString(temp);
									break;
								}
							}
						}
					}
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
							for (auto &c : input_focus) {
								c.second.first->text.setString("");
							}
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
								// deselect all other buttons.
								for (auto &v : modes) {
									v.second->text.setFillColor(sf::Color::Black);
								}
								c.second->text.setFillColor(sf::Color::Green);

								mode_selected = c.first;
								break;
							}
						}
						for (auto &c : input_focus) {
							if (check_button_clicked(c.second.first->rect)) {
								// unfocus any other inputs.
								for (auto &v : input_focus) {
									v.second.second = false;
									v.second.first->rect.setFillColor(sf::Color::White);
								}
								c.second.second = true;
								c.second.first->rect.setFillColor(sf::Color::Green);
								break;
							}
						}
						if (check_button_clicked(countdown_input.rect)) {
							countdown_mode = !countdown_mode;

							if (countdown_mode) {
								countdown_input.rect.setFillColor(sf::Color::Green);
								input_focus["target_count"].first->rect.setPosition(sf::Vector2f(0, 0));
								input_focus["target_count"].first->rect.setSize(sf::Vector2f(0, 0));
								input_focus["time"].first->rect.setSize(sf::Vector2f(200, 50));
								input_focus["time"].first->rect.setPosition(time_text.getPosition().x + 250,
																			time_text.getPosition().y);
							} else {
								countdown_input.rect.setFillColor(sf::Color::Red);
								input_focus["time"].first->rect.setPosition(sf::Vector2f(0, 0));
								input_focus["time"].first->rect.setSize(sf::Vector2f(0, 0));
								input_focus["target_count"].first->rect.setSize(sf::Vector2f(200, 50));
								input_focus["target_count"].first->rect.setPosition(
																					targets_text.getPosition().x + 250, targets_text.getPosition().y);
							}

							input_focus["target_count"].first->text.setPosition(
																				input_focus["target_count"].first->rect.getPosition().x,
																				input_focus["target_count"].first->rect.getPosition().y);
							input_focus["time"].first->text.setPosition(
																		input_focus["time"].first->rect.getPosition().x,
																		input_focus["time"].first->rect.getPosition().y);

							// deselect all inputs.
							for (auto &c : input_focus) {
								c.second.second = false;
								c.second.first->text.setString("");
								c.second.first->rect.setFillColor(sf::Color::White);
							}

						} else if (check_button_clicked(begin_button.rect)) {
							if ((!countdown_mode && !input_focus["target_count"].first->text.getString().isEmpty()) ||
								(countdown_mode && !input_focus["time"].first->text.getString().isEmpty())) {
								// targets_on_screen = std::stoi(std::string(input_focus["time"].first->text.getString()));
								prepare_session();
							}
						} else if (check_button_clicked(back_button.rect)) {
							current_screen = MAIN_MENU;
						}
					} else if (current_screen == SESSION) {
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
			if (targets_hit == max_targets) {
				end_session();
				current_screen = SUMMARY;
			} else {
				draw_session();
			}
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
	if (countdown_mode) {
		window.draw(time_text);
		window.draw(input_focus["time"].first->rect);
		window.draw(input_focus["time"].first->text);
	} else {
		window.draw(targets_text);
		window.draw(input_focus["target_count"].first->rect);
		window.draw(input_focus["target_count"].first->text);
	}
	window.draw(input_focus["on_screen_count"].first->rect);
	window.draw(input_focus["on_screen_count"].first->text);
	window.draw(countdown_input.rect);
	window.draw(begin_button.rect);
	window.draw(back_button.rect);
	window.draw(selection_title_text);
	window.draw(modes[CLASSIC]->text);
	window.draw(modes[PRECISION]->text);
	window.draw(modes[SPEED]->text);
	window.draw(on_screen_text);
	window.draw(countdown_text);

	window.draw(countdown_input.text);
	window.draw(begin_button.text);
	window.draw(back_button.text);
}

void Video::draw_session()
{
	std::ostringstream timer_ss;
	timer_ss.precision(2);

	if (countdown_mode) {
		time_remaining = (start_time - sf::seconds(current_session_timer.getElapsedTime().asSeconds())).asSeconds();
		if (time_remaining <= 0.0f) {
			end_session();
			current_screen = SUMMARY;
			return;
		}
		timer_ss << std::fixed << time_remaining;
		timer_text.setString("Remaining: " + timer_ss.str());

	} else {
		timer_ss << std::fixed << current_session_timer.getElapsedTime().asSeconds();
		timer_text.setString("Elapsed: " + timer_ss.str());
		window.draw(*targets[targets_hit]);
	}

	window.draw(targets_hit_text);
	window.draw(accuracy_text);
	window.draw(timer_text);

}

void Video::draw_summary()
{
	window.draw(summary_title_text);
	window.draw(summary_text);
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
	selection_title_text.setFont(font);
	selection_title_text.setCharacterSize(40);
	selection_title_text.setString("Select Mode");
	selection_title_text.setStyle(sf::Text::Bold);
	selection_title_text.setPosition(10, 10);
	selection_title_text.setFillColor(sf::Color::White);

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
	modes[PRECISION]->text.setString("Precision\n(not implemented)");
	modes[PRECISION]->text.setFillColor(sf::Color::Black);
	modes[PRECISION]->text.setCharacterSize(24);
	modes[PRECISION]->text.setPosition(
									   sf::Vector2f(modes[PRECISION]->rect.getPosition().x, modes[PRECISION]->rect.getPosition().y));

	// speed
	modes[SPEED]->rect.setSize(sf::Vector2f(200, 100));
	modes[SPEED]->rect.setPosition(sf::Vector2f(600, 100));
	modes[SPEED]->text.setFont(font);
	modes[SPEED]->text.setString("Speed\(not implemented)");
	modes[SPEED]->text.setFillColor(sf::Color::Black);
	modes[SPEED]->text.setCharacterSize(24);
	modes[SPEED]->text.setPosition(
								   sf::Vector2f(modes[SPEED]->rect.getPosition().x, modes[SPEED]->rect.getPosition().y));

	targets_text.setFont(font);
	targets_text.setString("Number of Targets:");
	targets_text.setFillColor(sf::Color::White);
	targets_text.setCharacterSize(24);
	targets_text.setPosition(sf::Vector2f(0, 250));
	input_focus["target_count"].first->rect.setSize(sf::Vector2f(200, 50));
	input_focus["target_count"].first->rect.setPosition(
														sf::Vector2f(targets_text.getPosition().x + 250, targets_text.getPosition().y));
	input_focus["target_count"].first->text.setFont(font);
	input_focus["target_count"].first->text.setFillColor(sf::Color::Black);
	input_focus["target_count"].first->text.setCharacterSize(24);
	input_focus["target_count"].first->text.setPosition(
														sf::Vector2f(input_focus["target_count"].first->rect.getPosition().x,
																	 input_focus["target_count"].first->rect.getPosition().y));

	on_screen_text.setFont(font);
	on_screen_text.setString("Targets on Screen:");
	on_screen_text.setFillColor(sf::Color::White);
	on_screen_text.setCharacterSize(24);
	on_screen_text.setPosition(sf::Vector2f(0, 350));
	input_focus["on_screen_count"].first->rect.setSize(sf::Vector2f(200, 50));
	input_focus["on_screen_count"].first->rect.setPosition(
														   sf::Vector2f(on_screen_text.getPosition().x + 250, on_screen_text.getPosition().y));
	input_focus["on_screen_count"].first->text.setFont(font);
	input_focus["on_screen_count"].first->text.setFillColor(sf::Color::Black);
	input_focus["on_screen_count"].first->text.setCharacterSize(24);
	input_focus["on_screen_count"].first->text.setPosition(
														   sf::Vector2f(input_focus["on_screen_count"].first->rect.getPosition().x,
																		input_focus["on_screen_count"].first->rect.getPosition().y));

	countdown_text.setFont(font);
	countdown_text.setString("Countdown Mode?:");
	countdown_text.setFillColor(sf::Color::White);
	countdown_text.setCharacterSize(24);
	countdown_text.setPosition(sf::Vector2f(0, 450));
	countdown_input.rect.setSize(sf::Vector2f(50, 50));
	countdown_input.rect.setFillColor(sf::Color::Red);
	countdown_input.rect.setPosition(
									 sf::Vector2f(countdown_text.getPosition().x + 250, countdown_text.getPosition().y));
	countdown_input.text.setFont(font);
	countdown_input.text.setCharacterSize(24);
	countdown_input.text.setPosition(
									 sf::Vector2f(countdown_input.rect.getPosition().x + 15, countdown_input.rect.getPosition().y));

	time_text.setFont(font);
	time_text.setString("Time (seconds):");
	time_text.setFillColor(sf::Color::White);
	time_text.setCharacterSize(24);
	time_text.setPosition(sf::Vector2f(0, 250));

	input_focus["time"].first->rect.setSize(sf::Vector2f(0, 0));
	input_focus["time"].first->rect.setPosition(sf::Vector2f(0, 0));
	input_focus["time"].first->text.setFont(font);
	input_focus["time"].first->text.setFillColor(sf::Color::Black);
	input_focus["time"].first->text.setCharacterSize(24);
	input_focus["time"].first->text.setPosition(
												sf::Vector2f(input_focus["time"].first->rect.getPosition().x,
															 input_focus["time"].first->rect.getPosition().y));

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
	summary_title_text.setFont(font);
	summary_title_text.setCharacterSize(40);
	summary_title_text.setString("Summary");
	summary_title_text.setPosition(10, 10);

	summary_text.setFont(font);
	summary_text.setCharacterSize(24);
	summary_text.setFillColor(sf::Color::White);
	summary_text.setPosition(10, 100);

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
		if (countdown_mode) {
			start_time = sf::seconds(std::stof(std::string(input_focus["time"].first->text.getString())));
			max_targets = 5;
		} else {
			max_targets = std::stoi(std::string(input_focus["target_count"].first->text.getString()));
		}
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
	std::ostringstream acc_ss;
	acc_ss.precision(2);
	acc_ss << std::fixed
		   << static_cast<double>(targets_hit) /
		(static_cast<double>(targets_hit) + static_cast<double>(targets_missed)) * 100.0;
	summary_text.setString("Time: " + std::to_string(current_session_timer.getElapsedTime().asSeconds()) +
						   " seconds"
						   "\nTotal Targets Hit: " +
						   std::to_string(targets_hit) + "\nMisses: " + std::to_string(targets_missed) +
						   "\nAccuracy: " + acc_ss.str() + "%");

	targets_hit = 0;
	targets_missed = 0;
	targets_hit_text.setString("Targets hit: " + std::to_string(targets_hit));
	accuracy_text.setString("Accuracy: 100.00%");
}

void Video::randomise_targets(const int &amount)
{
	if (!countdown_mode) {
		targets.clear();
	}

	// temporarily using rectangles until circle collision detection is implemented.
	for (int i = 0; i <= amount; ++i) {
		targets.push_back(std::make_unique<sf::RectangleShape>());
		targets[i]->setSize(sf::Vector2f(40, 40));
		targets[i]->setPosition(
								sf::Vector2f(rand() % (window.getSize().x - static_cast<int>(targets[i]->getSize().x)),
											 rand() % (window.getSize().y - static_cast<int>(targets[i]->getSize().y))));
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
