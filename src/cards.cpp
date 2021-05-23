/* Cards.cpp

   Contains all class implemetations
*/

#include "testmatch/cards.hpp"

#include "testmatch/enums.hpp"
#include "testmatch/helpers.hpp"
#include "testmatch/team.hpp"

#include <cmath>
#include <exception>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>

/*
    Dimissial implementations
*/
Dismissal::Dismissal(DismType c_mode, Player* c_bowler, Player* c_fielder) {
    mode = c_mode;
    bowler = nullptr;
    fielder = nullptr;

    if (c_mode != run_out) {
        bowler = c_bowler;
    }

    if (c_mode == caught || c_mode == run_out || c_mode == stumped) {
        fielder = c_fielder;
    }
}

std::string Dismissal::print_dism() {
    // TODO: include options for different print formats
    std::string output, bowl_name, field_name;

    if (bowler != nullptr) {
        bowl_name = bowler->get_last_name();
    }
    if (fielder != nullptr) {
        field_name = fielder->get_last_name();
    }

    // Unencode mode of dismissal
    std::string str_mode = str(mode);

    if (str_mode == "b" || str_mode == "lbw") {
        output = str_mode + " " + bowl_name;
    } else if (str_mode == "c" || str_mode == "st") {
        output = str_mode + " " + field_name + " b " + bowl_name;
    } else if (str_mode == "c&b") {
        output = str_mode + " " + bowl_name;
    } else {
        output = "run out (" + field_name + ")";
    }

    return output;
}

DismType Dismissal::get_mode() { return mode; }

Player* Dismissal::get_bowler() { return bowler; }

Player* Dismissal::get_fielder() { return fielder; }

/*
    Fatigue implementations
*/
// Distribution parameters
double Fatigue::MEAN_PACE_FATIGUE = 5;
double Fatigue::MEAN_SPIN_FATIGUE = 0.5;
double Fatigue::EXTRA_PACE_PENALTY = 1;
double Fatigue::VAR_PACE_FATIGUE = 1;
double Fatigue::VAR_SPIN_FATIGUE = 0.1;

// Uniform number generators
std::random_device Fatigue::RD;
std::mt19937 Fatigue::GEN{RD()};

Fatigue::Fatigue(BowlType c_bowl_type) : value(0) {

    // Set up sampling distribution
    double mean, var;
    if (is_slow_bowler(c_bowl_type)) {
        mean = MEAN_SPIN_FATIGUE;
        var = VAR_SPIN_FATIGUE;
    } else {
        mean = MEAN_PACE_FATIGUE;
        var = VAR_PACE_FATIGUE;

        // additional fatigue penalty for out-and-out fast bowlers
        if (c_bowl_type == fast) {
            mean += EXTRA_PACE_PENALTY;
        }
    }

    dist = new std::normal_distribution<double>(mean, sqrt(var));
}

double Fatigue::get_value() { return value; }

void Fatigue::ball_bowled() { value += (*dist)(GEN); }

void Fatigue::wicket() {
    // Player gets a boost
    if (value > 0)
        value -= 3 * dist->mean();
}

void Fatigue::rest(double time) {
    // Ease fatigue
    if (value > 0)
        value -= 3 * dist->mean();
}

Fatigue::~Fatigue() { delete dist; }

/*
    PlayerCard implementations
*/
PlayerCard::PlayerCard(Player* c_player, int c_order)
    : player(c_player), order(c_order) {}

Player* PlayerCard::get_player_ptr() { return player; }

int PlayerCard::get_order() { return order; }
/*
    BatterCard implementations
*/
BatterCard::BatterCard(Player* c_player, int c_order)
    : PlayerCard(c_player, c_order) {
    // Assume that batter has arrived at crease when object is created

    // Career averages
    stats.bat_avg = c_player->get_bat_avg();
    stats.strike_rate = c_player->get_bat_sr();

    // Batting hand
    // false: right, true: left
    stats.bat_arm = c_player->get_bat_arm();

    stats.runs = 0;
    stats.balls = 0;
    stats.fours = 0;
    stats.sixes = 0;

    active = false;
    out = false;
}

BatStats BatterCard::get_sim_stats() { return stats; }

bool BatterCard::is_active() { return active; }

void BatterCard::activate() {
    if (!active) {
        active = true;
    } else {
        // Throw exception - Batter can only be activated once
    }
}

void BatterCard::update_score(std::string outcome) {

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

    } else if (outcome.substr(1, 2) == "nb") {
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

void BatterCard::dismiss(DismType d_mode, Player* d_bowler, Player* d_fielder) {
    // Construct Dismissal structure
    dism = new Dismissal(d_mode, d_bowler, d_fielder);
    out = true;
}

std::string BatterCard::print_card(void) {
    std::string output = player->get_full_initials() + " ";

    // Dismissal
    if (out) {
        output += dism->print_dism() + " ";
    } else {
        output += "not out ";
    }

    // Stats
    output += std::to_string(stats.runs) + " (" + std::to_string(stats.balls) +
              "b " + std::to_string(stats.fours) + "x4 " +
              std::to_string(stats.sixes) + "x6) SR: ";

    if (stats.balls == 0) {
        output += "-";
    } else {
        double sr = 100 * stats.runs / (double)stats.balls;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << sr;
        output += ss.str();
    }

    return output;
}

std::string BatterCard::print_short(void) {
    std::string output = player->get_full_initials() + " " +
                         std::to_string(stats.runs) + " (" +
                         std::to_string(stats.balls) + ")";

    return output;
}

std::string BatterCard::print_dism(void) {
    if (out) {
        return dism->print_dism();
    } else {
        return "not out";
    }
}

BatterCard::~BatterCard() {
    if (out)
        delete dism;
}

/*
    BatterCard implementations
*/
BowlerCard::BowlerCard(Player* c_player, int c_order)
    : PlayerCard(c_player, c_order), tiredness(c_player->get_bowl_type()) {
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

    competency = DETERMINE_COMPETENCY(c_player);
}

int BowlerCard::DETERMINE_COMPETENCY(Player* player) {
    if (player->get_innings() == 0) {
        // Debut case - check role
        // TODO: Implement this

        return 2;
    }

    double avg_balls_per_match =
        (double)player->get_balls_bowled() / (double)player->get_innings();

    if (avg_balls_per_match > 30) {
        // Full-time if bowled more than 5 overs per innings on average
        return 0;
    } else if (avg_balls_per_match > 0) {
        // Part-time bowler
        return 1;
    } else {
        // Never bowls
        return 2;
    }
}

BowlStats BowlerCard::get_sim_stats(void) { return stats; }

void BowlerCard::start_new_spell() {
    stats.spell_balls = 0;
    stats.spell_overs = 0;
    stats.spell_maidens = 0;
    stats.spell_runs = 0;
    stats.spell_wickets = 0;

    active = true;
}

double BowlerCard::get_tiredness() { return tiredness.get_value(); }

int BowlerCard::get_competency() { return competency; }

void BowlerCard::over_rest() {
    active = false;
    tiredness.rest(0);
}

std::string BowlerCard::print_card(void) {
    std::string output = player->get_full_initials() + " ";

    output += std::to_string(stats.overs);
    if (stats.over_balls > 0)
        output += "." + std::to_string(stats.over_balls);
    output += "-" + std::to_string(stats.maidens) + "-";
    output += std::to_string(stats.runs) + "-";
    output += std::to_string(stats.wickets);

    return output;
}

std::string BowlerCard::print_spell(void) {
    std::string output = player->get_full_initials() + " ";

    // TODO: Implement

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

void BowlerCard::update_score(std::string outcome) {

    stats.balls++;
    tiredness.ball_bowled();

    if (outcome == "W") {
        stats.wickets++;
        stats.spell_wickets++;
        add_ball();
        tiredness.wicket();

    } else if (outcome.length() == 1) {
        stats.runs += stoi(outcome);
        stats.spell_runs += stoi(outcome);

        if (outcome != "0") {
            is_maiden = false;
        }

        add_ball();

    } else if (outcome.substr(1, 2) == "nb") {
        stats.runs += outcome.at(0) - '0';
        stats.spell_runs += outcome.at(0) - '0';
        is_maiden = false;

    } else if (outcome.substr(1, 2) == "wd") {
        stats.runs += outcome.at(0) - '0';
        stats.spell_runs += outcome.at(0) - '0';
        is_maiden = false;

    } else if (outcome.back() == 'b') {
        // Byes or leg byes
        add_ball();
    }

    // Update fatigue
    tiredness.ball_bowled();
}

template <typename T>
PlayerCard** sort_array(PlayerCard** list, int len,
                        T (Player::*sort_val)() const) {
    // Extract player pointers from cards;
    Player** ply_unsrt = new Player*[len];
    for (int i = 0; i < len; i++)
        ply_unsrt[i] = list[i]->get_player_ptr();

    // Sort by desired statistic
    Player** ply_srt = sort_array<T>(ply_unsrt, len, sort_val);

    // Sort PlayerCard pointers by Player sorting
    PlayerCard** sorted = new PlayerCard*[len];
    for (int i = 0; i < len; i++) {
        int itt = 0;
        while (itt < len && ply_srt[i] != list[itt]->get_player_ptr())
            itt++;

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
    delete[] ply_unsrt;
    delete[] ply_srt;

    return sorted;
}

BatterCard** create_batting_cards(Team* team) {
    BatterCard** cards = new BatterCard*[11];
    for (int i = 0; i < 11; i++) {
        cards[i] = new BatterCard(team->players[i], i);
    }

    return cards;
}
BowlerCard** create_bowling_cards(Team* team) {
    BowlerCard** cards = new BowlerCard*[11];
    for (int i = 0; i < 11; i++) {
        cards[i] = new BowlerCard(team->players[i], i);
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
    } else {
        last->next = ball;
        last = ball;
    }

    // Check if legal delivery
    if (ball->legal) {
        num_legal_delivs++;
    }

    num_balls++;
}

void Over::set_next(Over* p_next) { next = p_next; }

int Over::get_over_num() { return over_num; }

int Over::get_num_balls() { return num_balls; }

int Over::get_num_legal_delivs() { return num_legal_delivs; }

Ball* Over::get_first() { return first; }

Ball* Over::get_last() { return last; }

Over* Over::get_next() { return next; }

Over::~Over() {
    // Delete each ball iteratively
    delete_linkedlist<Ball>(first);
}

//~~~~~~~~~~~~~~ Extras implementations ~~~~~~~~~~~~~~//
Extras::Extras() : byes(0), legbyes(0), noballs(0), wides(0) {}

bool Extras::update_score(std::string outcome) {
    int runs = outcome.front() - '0';
    std::string query = outcome.substr(1, 2);

    if (outcome.length() == 1) {
        // Runs off bat, legal delivery
        return true;
    } else if (query == "lb") {
        // Legbyes, legal delivery
        legbyes += runs;
        return true;

    } else if (query == "nb") {
        // No ball, illegal delivery
        noballs += runs;
        return false;
    } else if (query == "wd") {
        // Wides, illegal delivery
        wides += runs;
        return false;
    } else {
        // Byes, legal delivery
        byes += runs;
        return true;
    }

    /**
     * There is a peculiar case which I am not sure how
     * to handle; What if legbyes or byes occur on a no ball?
     * By my current implementation, this will be recorded as
     * runs off the bat and awarded to the batter (ignoring the
     * additional no ball run). The ball-by-ball data I am using
     * to develop the delivery model does not make this distinguish,
     * so as it stands this is how it is done.
     **/
}

// Print methods
std::string Extras::print() const {
    std::vector<std::string> strings;

    if (byes > 0) {
        strings.push_back("b " + std::to_string(byes));
    }

    if (legbyes > 0) {
        strings.push_back("lb " + std::to_string(legbyes));
    }

    if (noballs > 0) {
        strings.push_back("nb " + std::to_string(noballs));
    }

    if (wides > 0) {
        strings.push_back("w " + std::to_string(wides));
    }

    return join_str(strings, ", ");
}

int Extras::total() const { return byes + legbyes + noballs + wides; }

std::string FOW::print() {
    std::string output = std::to_string(runs) + "-" + std::to_string(wkts) +
                         " (" + batter->get_full_name() + ", " +
                         std::to_string(overs) + "." + std::to_string(balls) +
                         " ov)";

    return output;
}

//~~~~~~~~~~~~~~ Partnership Implementations ~~~~~~~~~~~~~~//
Partnership::Partnership(Player* c_bat1, Player* c_bat2)
    : runs(0), bat1(c_bat1), bat2(c_bat2), bat1_runs(0), bat1_balls(0),
      bat2_runs(0), bat2_balls(0), not_out(true) {}

Player* Partnership::get_bat1() { return bat1; }
Player* Partnership::get_bat2() { return bat2; }

unsigned int Partnership::get_runs() { return runs; }

unsigned int Partnership::get_balls() { return bat1_balls + bat2_balls; }

bool Partnership::get_not_out() { return not_out; }

void Partnership::add_runs(unsigned int n_runs, bool scorer, bool add_ball) {
    runs += n_runs;
    int ab, ar;
    if (add_ball) {
        ab = 1;
        ar = n_runs;
    } else {
        ab = 0;
        ar = 0;
    }
    if (scorer) {
        bat2_runs += ar;
        bat2_balls += ab;
    } else {
        bat1_runs += ar;
        bat1_balls += ab;
    }
}

void Partnership::end() { not_out = false; }

/*=======*/

MatchResult::MatchResult(ResultType c_type, Team* c_winner,
                         unsigned int c_margin)
    : type(c_type), winner(c_winner), margin(c_margin) {
    // Ensure that no winner or margin is set for a draw or tie
    if (type == draw || type == tie) {
        winner = nullptr;
        margin = 0;
    }
}

ResultType MatchResult::get_type() { return type; }

Team* MatchResult::get_winner() { return winner; }

unsigned int MatchResult::get_margin() { return margin; }

std::string MatchResult::print() {
    switch (type) {
        case draw:
            return "Match Drawn";
        case win_chasing:
            return winner->name + " won by " + std::to_string(margin) +
                   " wickets";
        case win_bowling:
            return winner->name + " won by " + std::to_string(margin) + " runs";
        case win_innings:
            return winner->name + " won by an innings and " +
                   std::to_string(margin) + " runs";
        case tie:
            return "Match tied";
        default:
            // Raise exception - unknown value for type
            throw(std::invalid_argument("Undefined ResultType value."));
    }
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

bool Milestone::is_permitted(int value) { return true; }

// Getters
Player* Milestone::get_player() { return player; }

int Milestone::get_value() { return value; }

std::string Milestone::get_desc() { return desc; }

//
///* BatMilestone implementations */
// BatMilestone::BatMilestone() {
//
//}