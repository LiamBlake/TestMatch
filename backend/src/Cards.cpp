/* Cards.cpp

   Contains all class implemetations 
*/

#include "Player.h"
#include "Cards.h"
#include "Utility.h"

#include <string>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <exception>

using namespace std;


/* 
	Dimissial implementations
*/
Dismissal::Dismissal(int c_mode, Player* c_bowler, Player* c_fielder) {
	// Ensure dismissial mode is valid
	if (unencode_dism(c_mode) == "-") {
		throw invalid_argument("c_mode must correspond to a valid dismissial. See encode_dism in Utility.h for a list of valid encodings.");
	}
	
	mode = c_mode;
	bowler = nullptr;
	fielder = nullptr;

	if (c_mode != encode_dism("ro")) {
		bowler = c_bowler;
	}

	if (c_mode == encode_dism("c") || c_mode == encode_dism("ro") || c_mode == encode_dism("st")) {
		fielder = c_fielder;
	}
}


string Dismissal::print_dism() {
	// TODO: include options for different print formats
	string output, bowl_name, field_name;

	if (bowler != nullptr) {
		bowl_name = bowler->get_last_name();
	}
	if (fielder != nullptr) {
		field_name = fielder->get_last_name();
	}

	// Unencode mode of dismissal
	string str_mode = unencode_dism(mode);

	if (str_mode == "b" || str_mode == "lbw") {
		output = str_mode + " " + bowl_name;
	}
	else if (str_mode == "c" || str_mode == "st") {
		output = str_mode + " " + field_name + " b " + bowl_name;
	} else if (str_mode == "c&b") {
		output = str_mode + " " + bowl_name;
	} else {
		output = "run out (" + field_name + ")";
	}

	return output;

}

int Dismissal::get_mode() {
	return mode;
}

Player* Dismissal::get_bowler() {
	return bowler;
}

Player* Dismissal::get_fielder() {
	return fielder;
}


/*
	Fatigue implementations
*/
Fatigue::Fatigue(int c_bowl_type) : value(0), bowl_type(c_bowl_type) {}

double Fatigue::get_value() {
	return value;
}

void Fatigue::ball_bowled() {}
void Fatigue::wicket() {}
void Fatigue::rest(double time) {}



/*
	PlayerCard implementations
*/
PlayerCard::PlayerCard(Player* c_player) {
	player = c_player;
}


Player* PlayerCard::get_player_ptr() {
	return player;
}


/*
	BatterCard implementations
*/
BatterCard::BatterCard(Player* c_player) : PlayerCard(c_player) {
	// Assume that batter has arrived at crease when object is created

	// Career averages
	stats.bat_avg = c_player->get_bat_avg();
	stats.strike_rate = c_player->get_bat_sr();

	// Batting hand
	// false: right, true: left
	stats.bat_hand = c_player->get_bat_hand();

	stats.runs = 0;
	stats.balls = 0;
	stats.fours = 0;
	stats.sixes = 0;

	active = false;
	out = false;

}

BatStats BatterCard::get_sim_stats() {
	return stats;
}

bool BatterCard::is_active() {
	return active;
}

void BatterCard::activate() {
	if (!active) {
		active = true;
	} else {
		// Throw exception - Batter can only be activated once
	}
}

void BatterCard::update_score(string outcome) {

	if (outcome == "W") {
		stats.balls++;
		out = true;
		
	} else if (outcome == "0") {
		// Dot ball
		stats.balls++;

	} else if (outcome == "1") {
		// 1 run
		stats.runs++;
		stats.balls++;

	} else if (outcome == "2") {
		// 1 run
		stats.runs += 2;
		stats.balls++;

	} else if (outcome == "3") {
		// 1 run
		stats.runs += 3;
		stats.balls++;

	} else if (outcome == "4") {
		// 1 run
		stats.runs += 4;
		stats.balls++;
		stats.fours++;

	} else if (outcome == "5") {
		// Overthrows - 5 fives 
		stats.runs += 5;
		stats.balls++;
		stats.fours++;

		// Assume extra runs came from boundary

	} else if (outcome == "6") {
		// 6 runs
		stats.runs += 6;
		stats.balls++;
		stats.sixes++;

	} else if (outcome.substr(1,2) == "nb") {
		int runs = outcome.at(0) - '0' - 1;
		stats.runs += runs;
		stats.balls++;

		if (runs == 4) {
			stats.fours++;
		} else if (runs == 6) {
			stats.sixes++;
		}

	} else if (outcome.back() == 'b') {
		// Byes or leg byes
		stats.balls++;

	}

	// Do nothing on wides

}


void BatterCard::dismiss(int d_mode, Player* d_bowler, Player* d_fielder) {
	// Construct Dismissal structure
	dism = new Dismissal(d_mode, d_bowler, d_fielder);
	out = true;

}


string BatterCard::print_card(void) {
	string output = player->get_full_initials() + " ";

	// Dismissal
	if (out) {
		output += dism->print_dism() + " ";
	} else {
		output += "not out ";
	}

	// Stats
	output += to_string(stats.runs) + " (" + to_string(stats.balls) + "b " + to_string(stats.fours) + "x4 " + to_string(stats.sixes) + "x6) SR: ";
	
	if (stats.balls == 0) {
		output += "-";
	} else {
		double sr = 100 * stats.runs / (double) stats.balls;
		stringstream ss;
		ss << fixed << setprecision(2) << sr;
		output += ss.str();
	}

	return output;
}


BatterCard::~BatterCard() {
	delete dism;
}




/*
	BatterCard implementations
*/
BowlerCard::BowlerCard(Player* c_player) : PlayerCard(c_player), tiredness(c_player->get_bowl_type()) {
	stats.bowl_avg = c_player->get_bowl_avg();
	stats.strike_rate = c_player->get_bowl_sr();
	stats.bowl_type = c_player->get_bowl_type();		

	stats.balls = 0;
	stats.overs = 0;
	stats.over_balls = 0;
	stats.maidens = 0;
	stats.runs = 0;
	stats.wickets = 0;

	stats.spell_balls = 0;
	stats.spell_overs = 0;
	stats.spell_maidens = 0;
	stats.spell_runs = 0;
	stats.spell_wickets = 0;

	is_maiden = true;

	active = false;
}

BowlStats BowlerCard::get_sim_stats(void) {
	return stats;
}

void BowlerCard::start_new_spell() {
	stats.spell_balls = 0;
	stats.spell_overs = 0;
	stats.spell_maidens = 0;
	stats.spell_runs = 0;
	stats.spell_wickets = 0;

	active = true;

}

void BowlerCard::over_rest() {
	active = false;
}


string BowlerCard::print_card(void) {
	string output = player->get_full_initials() + " ";

	output += to_string(stats.overs) + "." + to_string(stats.over_balls) + "-";
	output += to_string(stats.maidens) + "-";
	output += to_string(stats.runs) + "-";
	output += to_string(stats.wickets);

	return output;
}

string BowlerCard::print_spell(void) {
	string output = player->get_full_initials() + " ";

	output += to_string(stats.spell_overs) + "." + to_string(stats.over_balls) + "-";
	output += to_string(stats.spell_maidens) + "-";
	output += to_string(stats.spell_runs) + "-";
	output += to_string(stats.spell_wickets);

	return output;
}


void BowlerCard::add_ball() {

	if (stats.over_balls == 5) {
		stats.overs++;
		stats.over_balls = 0;

		if (is_maiden) {
			stats.maidens++;
			stats.spell_maidens++;
			is_maiden = true;
		}

	} else {
		stats.over_balls++;
	}
}

void BowlerCard::update_score(string outcome) {

	stats.balls++;

	if (outcome == "W") {
		stats.wickets++;
		stats.spell_wickets++;
		add_ball();
		
	} else if (outcome.length() == 1) {
		stats.runs += stoi(outcome);
		stats.spell_runs += stoi(outcome);

		if (outcome != "0") {
			is_maiden = false;
		}
		
		add_ball();

	} else if (outcome.substr(1,2) == "nb") {
		stats.runs += outcome.at(0) - '0';
		stats.spell_runs += outcome.at(0) - '0';
		is_maiden = false;

	} else if (outcome.back() == 'w') {
		stats.runs += outcome.at(0) - '0';
		stats.spell_runs += outcome.at(0) - '0';
		is_maiden = false;

	} else if (outcome.back() == 'b') {
		// Byes or leg byes
		add_ball();
	} 

}


template <typename T>
PlayerCard** sort_array(PlayerCard** list, int len, T (Player::*sort_val)() const) {
	// Extract player pointers from cards;
	Player** ply_unsrt = new Player* [len];
	for (int i = 0; i < len; i++) ply_unsrt[i] = list[i]->get_player_ptr();

	// Sort by desired statistic
	Player** ply_srt = sort_array<T>(ply_unsrt, len, sort_val);

	// Sort PlayerCard pointers by Player sorting
	PlayerCard** sorted = new Player* [len];
	for (int i = 0; i < len; i++) {
		int itt = 0;
		while (ply_srt[i] != list[itt] && itt < len) itt++;

		if (itt == len) {
			// Something has gone terribly wrong, raise an exception
		}

		sorted[i] = list[itt];

		/** 
		 * Definitely not the most efficient method, but for now we are
		 * unlikely to need to sort arrays of more than 11 player pointers, so
		 * the inefficiency shouldn't be an issue. 
		 **/
	}

	// Free temporarily allocated memory
	delete[] ply_unsrt, ply_srt;

	return sorted;

}


BatterCard** create_batting_cards(Team team) {
	BatterCard** cards = new BatterCard * [11];
	for (int i = 0; i < 11; i++) {
		cards[i] = new BatterCard(team.players[i]);
	}

	return cards;
}

BowlerCard** create_bowling_cards(Team team) {
	BowlerCard** cards = new BowlerCard * [11];
	for (int i = 0; i < 11; i++) {
		cards[i] = new BowlerCard(team.players[i]);
	}

	return cards;
}



/* Over class implementations */
Over::Over(int c_over_num) {
    over_num = c_over_num;    

    num_balls = 0;
    num_legal_delivs = 0;

	first = last = nullptr;

}

/**
 * 
 * 
*/
void Over::add_ball(Ball* ball) {

    // Add to linked list
	if (last == nullptr) {
		first = last = ball;
	}
	else {
		last->next = ball;
		last = ball;
	}

    // Check if legal delivery
    if (ball->legal) {
        num_legal_delivs++;
    }

    num_balls++;


}

void Over::set_next(Over* p_next) {
	next = p_next;
}

int Over::get_over_num() {
	return over_num;
}

int Over::get_num_balls() {
	return num_balls;
}

int Over::get_num_legal_delivs() {
	return num_legal_delivs;
}

Ball* Over::get_first() {
	return first;
}

Ball* Over::get_last() {
	return last;
}

Over* Over::get_next() {
	return next;
}


// Print methods
std::string Extras::print() {


  std::vector<std::string> strings;
  
  if (byes > 0) {
	strings.push_back("b " + to_string(byes));
  }

	if (legbyes > 0) {
		strings.push_back("lb " + to_string(legbyes));
	}

	if (noballs > 0) {
		strings.push_back("nb " + to_string(noballs));
	}

	if (wides > 0) {
		strings.push_back("w " + to_string(wides));
	}
		
  return join_str(strings, ", ");
}

std::string FOW::print() {
  std::string output = to_string(runs) + "-"
  						  + to_string(wkts) + " ("
  						  + batter->get_full_name() + ", "
						  + to_string(overs) + "." + to_string(balls)
  						  + " ov)";

  return output;
}




/* Milestone implementation */
// Contructor
Milestone::Milestone(Player* c_player, int c_value) 
		: player(c_player), value(c_value), desc("") {
			if (is_permitted(c_value)) {
				value = c_value;
			}
		}

		// THIS WON'T WORK - NEED A BETTER SOLUTION
	

bool Milestone::is_permitted(int value) {
	return true;
}

// Getters
Player* Milestone::get_player() {
	return player;
}

int Milestone::get_value() {
	return value;
}

std::string Milestone::get_desc() {
	return desc;
}

//
///* BatMilestone implementations */
//BatMilestone::BatMilestone() {
//
//}