#include <SFML/Graphics.hpp>
#include <ctime>
#include <random>
#include <vector>
#include <sstream>
#include <map>
#include <memory>

#include "screen.hpp"
#include "mode.hpp"

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
	void draw_mode_selection();
	void draw_session();

	void set_up_main_menu_elements();
	void set_up_mode_selection_elements();
	void set_up_session_elements();

	// helper functions.
	bool check_button_clicked(const sf::RectangleShape &rect);
	bool check_target_clicked(const sf::CircleShape &target);
	void select_mode(const Mode &mode);
	void prepare_session();
	void randomise_targets(const int &amount);

	Screen current_screen = MAIN_MENU; // default screen to start on.
	sf::RenderWindow window;

	// main menu elements
	sf::RectangleShape play_rect, options_rect, quit_rect;
	sf::Text play_text, options_text, quit_text;
	sf::Font font;

	// mode selection elements
	sf::RectangleShape begin_rect, back_rect;
	sf::Text classic_text, precision_text, speed_text, begin_text, back_text;
	std::vector<std::pair<std::unique_ptr<sf::RectangleShape>, Mode>> modes;
	Mode mode_selected;

	// session elements
	std::vector<std::unique_ptr<sf::RectangleShape>> targets; // temporary
	// std::vector<std::unique_ptr<sf::CircleShape>> targets;
	int targets_hit = 0;
	int max_targets = 5; // to be chosen by the player.
	sf::Text targets_hit_text, score_text, timer_text;
	sf::Clock current_session_timer;
	sf::Time total_elapsed;
};
