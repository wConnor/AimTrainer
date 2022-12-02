#include "video.hpp"
#include <SFML/Graphics/CircleShape.hpp>

Video::Video()
{
	std::srand(std::time(nullptr));
}

bool Video::init()
{
	window.create(sf::VideoMode(800, 600), "AimTrainer");
	window.setVerticalSyncEnabled(true);

	if (!font.loadFromFile("SFProText-Regular.ttf")) {
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
							start_text.setString("Clicked!");
							targets_hit = 0;
							randomise_targets(10);
							current_screen = SESSION;
						} else if (check_button_clicked(options_rect)) {
							options_text.setString("Clicked!");
							current_screen = OPTIONS;
						} else if (check_button_clicked(quit_rect)) {
							window.close();
						}
					} else if (current_screen == SESSION) {
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
	window.draw(*targets[targets_hit]);
}

bool Video::check_button_clicked(const sf::RectangleShape &rect)
{
	sf::FloatRect cursor_rect(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y,
							  CURSOR_WIDTH, CURSOR_HEIGHT);

	return cursor_rect.intersects(rect.getGlobalBounds());
}

bool Video::check_target_clicked(const sf::CircleShape &target)
{
	// TODO: implement.
	return true;
}

void Video::randomise_targets(const int &amount)
{
	targets.clear();

	for (int i = 0; i != amount; ++i) {
		targets.push_back(std::make_unique<sf::CircleShape>());
		targets[i]->setRadius(30);
		targets[i]->setPointCount(30);
		targets[i]->setPosition(
								sf::Vector2f(rand() % window.getSize().x, rand() % window.getSize().y));
	}
}

Video::~Video()
{
}
