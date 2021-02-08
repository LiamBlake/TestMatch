// -*- lsst-c++ -*-
/* Player.h
 *
 * Interface for basic player storage types. Individual players and their relevant career statistics
 * 
*/

#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <iostream>


/**
 * @brief Storage for all career statistics of an individual player
 * 
 * Self-explanatory, contains basic data types storing each relevant statistic.
 * See each declaration for a more detailed explanation.
 * 
*/
struct Stats {
	// Batting statistics
	int innings;		// Number of innings batted over career
	double bat_avg;		// Batting average (average runs per dismissal)
	double bat_sr;		// Batting strike rate (average runs per 100 balls faced)

	// Bowling statistics
	int balls_bowled;	// Number of balls bowled over career
	double bowl_avg;	// Bowling average (average runs conceded per wicket)
	double bowl_sr;		// Bowling strike rate (average balls bowled per wicket)
	double bowl_econ;	// Bowling economy (average runs conceded per 6 balls bowled)

	// General descriptors
	bool bat_hand;		// Batting hand (false = right, true = left)
	int bowl_type;		// Bowling type, encoded as integer, see Utility.h for encodings

};


/**
 * @brief Storage for all detail describing a player
 * 
 * Stores all information needed to describe a player, including name, team and statistics. Values are stored privately and
 * accessed using getters, following typical OOP convention. Class also includes methods for formatting name (e.g. initials and last name,
 * full name, etc.), and getters for each statistic stored in the Stats object. 
*/
class Player
{
  private:
	// Names
	std::string first_name;		// First name of player, e.g. John
	std::string last_name;		// Surname of player, e.g. Smith
	std::string initials;		// Initials of first and middle names, e.g. for John Doe Smith, store JD

	Stats player_stats;		// Player career statistics

  public:
	// Explicit constructor
	Player(std::string c_first_name, std::string c_last_name, std::string c_initials, Stats stats);
	
	// Default destructor

	// Getters
	std::string get_initials() const;
	std::string get_last_name() const;

	// Return full first, last name
	std::string get_full_name() const;
	std::string get_full_initials() const;

	// Return Stats struct
	Stats get_stats() const;

	// Specific getters for career statistics, stored in player_stats
	int get_innings() const;
	double get_bat_avg() const;
	double get_bat_sr() const;
	int get_balls_bowled() const;
	double get_bowl_avg() const;
	double get_bowl_sr() const;
	double get_bowl_econ() const;
	bool get_bat_hand() const;
	int get_bowl_type() const;

};

/**
 * @brief Sorts a dynamic array of Player pointers by a passed statistic.
 * @tparam T Typename of statistic sorting by, return type of sort_val.
 * @param list Pointer to array of Player pointers to be sorted.
 * @param len Length of array to be sorted.
 * @param sort_val Function pointer to Player getter method of the desired statistic to sort the array by.
 * @return Pointer to a NEW sorted array of Player pointers, of size len. Dynamically allocated, must be deleted.
*/
template <typename T>
Player** sort_array(Player** list, int len, T (Player::*sort_val)() const);


/**
 * @brief Storage for a playing XI of Player objects
 * 
 * Stores 11 Player objects in order corresponding to a playing XI, and also includes team name and 
 * indices corresponding to specialist roles in the team. The XI is stored as a static array of 11 Player
 * pointers, ordered by the desired batting order (under standard conditions - see the functions get_nightwatch,
 * - in Simulation.h for situations where this batting order is changed). The team name is stored as a string, and
 * the struct also includes 4 integers storing indices pointing to the captain, wicketkeeper and the two opening 
 * bowlers in the playing XI array.
*/
struct Team {
	std::string name;
	Player* players[11];

	// Indices refer to players array
	int i_captain;
	int i_wk;
	int i_bowl1;
	int i_bowl2;
};



/**
 * @brief Prints to the console a nicely formatted playing XI from a Team struct.
 * @param os 
 * @param team Team object to be printed.
 * @return 
 * @relatesals Team
*/
std::ostream& operator<<(std::ostream& os, const Team& team);

#endif // PLAYER_H