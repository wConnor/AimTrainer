#include "video.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <iterator>

Video::Video()
{
	std::srand(std::time(nullptr));
}

bool Video::init()
{
	window.create(sf::VideoMode(800, 600), "AimTrainer");
	window.setVerticalSyncEnabled(true);

	if (!font.loadFromFile("../../assets/fonts/Roboto-Regular.ttf")) {
		return 1;
	}

	// main menu
	start_rect.setSize(sf::Vector2f(200, 50));
	start_rect.setPosition(sf::Vector2f(0, 100));

	options_rect.setSize(sf::Vector2f(200, 50));
	options_rect.setPosition(sf::Vector2f(0, 200));

	quit_rect.setSize(sf::Vector2f(200, 50));
	quit_rect.setPosition(sf::Vector2f(0, 300));

	start_text.setFont(font);
	start_text.setString("Start");
	start_text.setFillColor(sf::Color::Black);
	start_text.setCharacterSize(24);
	start_text.setPosition(sf::Vector2f(0, 100));

	options_text.setFont(font);
	options_text.setString("Options");
	options_text.setFillColor(sf::Color::Black);
	options_text.setCharacterSize(24);
	options_text.setPosition(sf::Vector2f(0, 200));

	quit_text.setFont(font);
	quit_text.setString("Quit");
	quit_text.setFillColor(sf::Color::Cyan);
	quit_text.setCharacterSize(24);
	quit_text.setPosition(sf::Vector2f(0, 300));

	// session
	targets_hit_text.setFont(font);
	targets_hit_text.setString("Targets hit: " + std::to_string(targets_hit));
	targets_hit_text.setFillColor(sf::Color::Cyan);
	targets_hit_text.setCharacterSize(24);
	targets_hit_text.setPosition(0, 0);

	timer_text.setFont(font);
	timer_text.setFillColor(sf::Color::Magenta);
	timer_text.setCharacterSize(24);
	timer_text.setPosition(0, 30);

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
						if (check_button_clicked(start_rect)) {
							targets_hit = 0;
							randomise_targets(max_targets);
							current_screen = SESSION;
							current_session_timer.restart();
						} else if (check_button_clicked(options_rect)) {
							current_screen = OPTIONS;
						} else if (check_button_clicked(quit_rect)) {
							window.close();
						}
					} else if (current_screen == SESSION) {
						if (targets_hit == max_targets - 1) {
							targets_hit++;
							current_screen = MAIN_MENU; // will be changed to stats screen
							targets_hit =
								0; // will be set to 0 in the draw_stats() function; NOT here.
							targets_hit_text.setString("Targets hit: " +
													   std::to_string(targets_hit));
						} else if (check_button_clicked(*targets[targets_hit])) {
							targets_hit++;
							targets_hit_text.setString("Targets hit: " +
													   std::to_string(targets_hit));
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
	window.draw(start_rect);
	window.draw(options_rect);
	window.draw(quit_rect);

	window.draw(start_text);
	window.draw(options_text);
	window.draw(quit_text);
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

void Video::randomise_targets(const int &amount)
{
	targets.clear();

	// temporarily using rectangles until circle collision is implemented.
	for (int i = 0; i <= amount; ++i) {
		targets.push_back(std::make_unique<sf::RectangleShape>());
		targets[i]->setPosition(
								sf::Vector2f(rand() % window.getSize().x, rand() % window.getSize().y));
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
