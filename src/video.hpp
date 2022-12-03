#include <SFML/Graphics.hpp>
#include <ctime>
#include <random>
#include <vector>
#include <sstream>

#include "screen.hpp"

constexpr int CURSOR_WIDTH = 5;
constexpr int CURSOR_HEIGHT = 5;

class Video
{
public:
	Video();
	bool init();
	bool set_resolution(const unsigned int &width, const unsigned int &height);
	virtual ~Video();

private:
	// functions to render different screens / sections.
	void draw_main_menu();
	void draw_session();

	// helper functions.
	bool check_button_clicked(const sf::RectangleShape &rect);
	bool check_target_clicked(const sf::CircleShape &target);
	void randomise_targets(const int &amount);

	Screen current_screen = MAIN_MENU; // default screen to start on.
	sf::RenderWindow window;

	// main menu elements
	sf::RectangleShape start_rect, options_rect, quit_rect;
	sf::Text start_text, options_text, quit_text;
	sf::Font font;

	// session elements
	std::vector<std::unique_ptr<sf::RectangleShape>> targets; // temporary
	// std::vector<std::unique_ptr<sf::CircleShape>> targets;
	int targets_hit = 0;
	int max_targets = 5; // to be chosen by the player.
	sf::Text targets_hit_text, score_text, timer_text;
	sf::Clock current_session_timer;
	sf::Time total_elapsed;
};
