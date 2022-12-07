#include <SFML/Graphics.hpp>
#include <ctime>
#include <random>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <memory>

#include "screen.hpp"
#include "mode.hpp"
#include "button.hpp"

constexpr int CURSOR_WIDTH = 10;
constexpr int CURSOR_HEIGHT = 10;

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
	void draw_options();
	void draw_mode_selection();
	void draw_session();
	void draw_summary();

	void set_up_main_menu_elements();
	void set_up_options_elements();
	void set_up_mode_selection_elements();
	void set_up_session_elements();
	void set_up_summary_elements();

	// helper functions.
	bool check_button_clicked(const sf::Sprite &sprite);
	bool check_button_clicked(const sf::RectangleShape &rect);
	bool check_target_clicked(const sf::CircleShape &target);
	void prepare_session();
	void end_session();
	void randomise_targets(const int &amount);

	// sfml
	Screen current_screen = MAIN_MENU; // default screen to start on.
	sf::RenderWindow window;
	bool custom_cursor = false;
	sf::Sprite cursor;

	// main menu elements
	Button play_button, options_button, quit_button;
	sf::Text title_text;
	sf::Font font;

	// options elements
	Button ok_options_button;
	sf::Texture default_cursor, plus_red;
	sf::Sprite cursor_crosshair, plus_crosshair;
	sf::Text options_text, crosshair_text;

	// mode selection elements
	bool countdown_mode = false;
	sf::Text selection_title_text, targets_text, on_screen_text, countdown_text, time_text;
	std::map<std::string, std::pair<std::unique_ptr<Button>, bool>> input_focus;
	Button begin_button, back_button, countdown_input;
	std::unordered_map<Mode, std::unique_ptr<Button>> modes;
	Mode mode_selected;

	// session elements
	std::vector<std::unique_ptr<sf::RectangleShape>> targets; // temporary
	// std::vector<std::unique_ptr<sf::CircleShape>> targets;
	int targets_hit = 0, targets_missed = 0;
	int max_targets = 10; // to be chosen by the player.
	sf::Text targets_hit_text, accuracy_text, score_text, timer_text;
	sf::Clock current_session_timer, countdown_timer;
	sf::Time start_time;
	float time_remaining;

	// summary elements
	sf::Text summary_title_text, summary_text;
	Button ok_summary_button;
};
