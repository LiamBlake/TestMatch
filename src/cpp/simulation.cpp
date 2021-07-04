#include "testmatch/simulation.hpp"

#include "testmatch/cards.hpp"
#include "testmatch/enums.hpp"
#include "testmatch/helpers.hpp"
#include "testmatch/matchtime.hpp"
#include "testmatch/models.hpp"
#include "testmatch/pregame.hpp"
#include "testmatch/team.hpp"

#include <cmath>
#include <exception>
#include <functional>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <utility>

//~~~~~~~~~~~~~~ Parameters ~~~~~~~~~~~~~~//
double FieldingManager::C_WK_PROB = 0.5;

//~~~~~~~~~~~~~~ BattingManager implementations ~~~~~~~~~~~~~~//
BattingManager::BattingManager() {
    // Mark each batter as inactive
    for (int i = 0; i < 11; i++) {
        batted[i] = false;
    }
}

void BattingManager::set_cards(BatterCard** c_cards) {
    for (int i = 0; i < 11; i++)
        cards[i] = c_cards[i];
}

BatterCard* BattingManager::next_ordered() {
    // Find the first batter in the ordered XI who is yet to bat
    int itt = 0;
    while (itt < 11 && batted[itt])
        itt++;

    if (itt == 11) {
        // Throw exception if all players have batted

    } else {
        batted[itt] = true;
        return cards[itt];
    }
}

BatterCard* BattingManager::nightwatch() { return nullptr; }

BatterCard* BattingManager::promote_hitter() { return nullptr; }

BatterCard* BattingManager::next_in(Innings* inns_obj) {
    // Start of innings
    if (inns_obj->balls == 0) {
        return next_ordered();
    }

    // For now, send next in
    return next_ordered();
}

//~~~~~~~~~~~~~~ BowlingManager implementations ~~~~~~~~~~~~~~//
BowlingManager::BowlingManager() : n_over_calls(0){};

void BowlingManager::set_cards(BowlerCard* c_cards[11]) {
    for (int i = 0; i < 11; i++) {
        cards[i] = c_cards[i];

        // Correct for "cheating" part-time bowlers - blow up bowling averages
        Player* ply_ptr = cards[i]->get_player_ptr();
        if (ply_ptr->get_innings() > 0 &&
            ply_ptr->get_balls_bowled() / ply_ptr->get_innings() < 1) {
            ply_ptr->inflate_bowl_avg();
        }
    }
}

/**
 *
 * Logistic curve model with midpoint at x = 180 and growth rate k = 0.2
 *
 */
double BowlingManager::take_off_prob(double fatigue) {
    return 1.0 / (1 + exp(-0.2 * (fatigue - 180)));
}

BowlerCard* BowlingManager::new_pacer(BowlerCard* ignore1,
                                      BowlerCard* ignore2) {
    // Find each (full-time) pace-bowler in XI and measure objective fatigue
    return search_best([ignore1, ignore2](BowlerCard* bc) {
        return !is_slow_bowler(bc->get_player_ptr()->get_bowl_type()) &&
               bc->get_competency() == 0 && (bc != ignore1) && (bc != ignore2);
    });
}

BowlerCard* BowlingManager::new_spinner(BowlerCard* ignore1,
                                        BowlerCard* ignore2) {
    // Find each (full-time) spinner in XI and measure objective fatigue
    return search_best([ignore1, ignore2](BowlerCard* bc) {
        return (is_slow_bowler(bc->get_player_ptr()->get_bowl_type()) &&
                (bc->get_competency() == 0) && (bc != ignore1) &&
                (bc != ignore2));
    });
}

BowlerCard* BowlingManager::part_timer(BowlerCard* ignore1,
                                       BowlerCard* ignore2) {
    // Find any part-time bowler
    return search_best([ignore1, ignore2](BowlerCard* bc) {
        return (bc->get_competency() == 1) && (bc != ignore1) &&
               (bc != ignore2);
    });
}

BowlerCard* BowlingManager::change_it_up(BowlerCard* ignore1,
                                         BowlerCard* ignore2) {
    // Shit's cooked: find anyone who doesn't bowl and send them in
    return search_best([ignore1, ignore2](BowlerCard* bc) {
        return bc->get_competency() == 2 && (bc != ignore1) && (bc != ignore2);
    });
}

BowlerCard* BowlingManager::any_fulltime(BowlerCard* ignore1,
                                         BowlerCard* ignore2) {
    return search_best([ignore1, ignore2](BowlerCard* bc) {
        return bc->get_competency() == 0 && (bc != ignore1) && (bc != ignore2);
    });
}

BowlerCard* BowlingManager::end_over(Innings* inns_obj) {
    // Rest all players who didn't bowl the over
    for (int i = 0; i < 11; i++) {
        BowlerCard* ptr = cards[i];
        if (ptr != inns_obj->bowl2)
            ptr->over_rest();
    }

    // Special case - new ball
    if (inns_obj->overs == 80 || inns_obj->overs == 81) {
        BowlerCard* new_bowl = new_pacer(inns_obj->bowl1, inns_obj->bowl2);
        if (new_bowl != nullptr)
            return new_bowl;
        else
            return inns_obj->bowl1;
        // Not going to do this check for the third, fourth, etc. new balls - at
        // this point, the fielding team are waiting for a declaration, begging
        // for death, or both.
    }

    // As the current partnership grows,
    //  probability of bringing on a part -
    //    time bowler increases
    // if (inns_obj->bowl1->get_competency() == 0 &&
    //    inns_obj->bowl2->get_competency() == 0 &&
    //    (double)rand() / (RAND_MAX) <
    //       1.0 /
    //         (1 +
    //        exp(-0.01 *
    //               (inns_obj->bat_parts[inns_obj->wkts]->get_runs() - 100)) -
    //         1.0 / (1 + exp(1)))) {
    //               return part_timer(inns_obj->bowl1, inns_obj->bowl2);
    //         }

    // Decide whether to take the current bowler off
    double top = take_off_prob(inns_obj->bowl1->get_tiredness());
    if (inns_obj->bowl1->get_competency() != 0)
        top *= 5; // Penalty for being a part time bowler
    if (((double)rand() / (RAND_MAX)) < 1 / top) {
        // Change bowler
        // For now, just get the best full-time bowler
        return any_fulltime(inns_obj->bowl1, inns_obj->bowl2);

    } else
        return inns_obj->bowl1;

    return inns_obj->bowl1;
}

//~~~~~~~~~~~~~~ FieldingManager implementations ~~~~~~~~~~~~~~//
FieldingManager::FieldingManager(int c_wk_idx) : wk_idx(c_wk_idx) {}

void FieldingManager::set_cards(Player* c_plys[11]) {
    for (int i = 0; i < 11; i++)
        players[i] = c_plys[i];
}

Player* FieldingManager::select_catcher(Player* bowler, DismType dism_type) {
    Player** potential;
    int n;

    std::string dism = str(dism_type);
    // Dismissals not involving a fielder
    if (dism == "b" || dism == "lbw" || dism == "c&b")
        return nullptr;

    // Stumping
    if (dism == "st")
        return players[wk_idx];

    if (dism == "ro") {
        potential = new Player*[11];
        n = 11;
    } else {
        potential = new Player*[10];
        n = 10;
    }

    // Don't need to do this every time
    double* cdf = new double[n];
    cdf[0] = 0;
    int j = 0;
    for (int i = 0; i < 11; i++) {
        if ((players[i] != bowler) || dism == "ro") {
            potential[j] = players[i];
            j++;
            if (j < n) {
                if (i == wk_idx)
                    cdf[j] = cdf[j - 1] + C_WK_PROB;
                else
                    cdf[j] = cdf[j - 1] + (1 - C_WK_PROB) / (n - 1);
            }
        }
    }

    // Randomly sample a fielder
    Player* fielder = sample_cdf<Player*>(potential, n, cdf);
    // Need to construct CDF giving more weighting to wk

    delete[] potential;
    delete[] cdf;
    return fielder;
}

//~~~~~~~~~~~~~~ Innings implementations ~~~~~~~~~~~~~~//
int Innings::NO_INNS = 0;

// Printing variables
bool Innings::AUSTRALIAN_STYLE = false;
std::string Innings::DIVIDER =
    "\n-----------------------------------------------"
    "------------------------------\n";
std::string Innings::BUFFER = "   ";

// Constructor
Innings::Innings(Team* c_team_bat, Team* c_team_bowl, int c_lead,
                 PitchFactors* c_pitch)
    : overs(0), balls(0), legal_delivs(0), team_score(0), team_bat(c_team_bat),
      team_bowl(c_team_bowl), lead(c_lead), wkts(0), pitch(c_pitch),
      man_field(c_team_bowl->i_wk), is_open(true) {

    NO_INNS++;
    inns_no = NO_INNS;

    // Create BatterCards/BowlerCards for each player
    batters = create_batting_cards(team_bat);
    bowlers = create_bowling_cards(team_bowl);

    // Initialise managers
    man_bat.set_cards(batters);
    man_bowl.set_cards(bowlers);
    man_field.set_cards(team_bowl->players);

    // Get opening batters
    BatterCard* bat1 = man_bat.next_in(this);
    BatterCard* bat2 = man_bat.next_in(this);

    // First on strike is chosen randomly
    if (((double)rand() / (RAND_MAX)) < 0.5) {
        striker = bat1;
        nonstriker = bat2;
    } else {
        striker = bat2;
        nonstriker = bat1;
    }

    // Active openers
    striker->activate();
    nonstriker->activate();

    // Get opening bowlers
    bowl1 = bowlers[team_bowl->i_bowl1];
    bowl2 = bowlers[team_bowl->i_bowl2];

    // Set-up the first over
    first_over = last_over = new Over(1);

    // Set up partnership for first wicket
    bat_parts[0] =
        new Partnership(bat1->get_player_ptr(), bat2->get_player_ptr());
    for (int i = 1; i < 10; i++)
        bat_parts[i] = nullptr;

    // Setup FOW array
    fow = new FOW[10];
}

// Private methods used in simulation process
void Innings::simulate_delivery() {
    // Pass game information to delivery model

    // Get outcome probabilities
    std::map<std::string, double> probs = prediction::delivery(
        striker->get_sim_stats(), bowl1->get_sim_stats(), {});

    // Simulate
    std::string outcome = sample_pf_map<std::string>(probs);

    std::pair<int, std::string> t_output;

    // Create a Ball object
    balls++;
    Ball* new_ball = new Ball;
    *new_ball = {bowl1->get_player_ptr(), striker->get_player_ptr(), outcome,
                 true, ""};

    // Update cards
    striker->update_score(outcome);
    bowl1->update_score(outcome);
    bool is_legal = extras.update_score(outcome);
    new_ball->legal = is_legal;

    // Add ball to over
    last_over->add_ball(new_ball);

    if (!is_quiet) {
        // Print commentary
        std::cout << comm_ball(overs, bowl1->get_player_ptr(),
                               striker->get_player_ptr(), outcome)
                  << std::endl;
    }

    // Handle each outcome case
    if (outcome == "W") {
        wkts++;
        legal_delivs++;

        // Randomly choose the type of dismissal
        std::map<DismType, double> dism_pf =
            prediction::wkt_type(bowl1->get_player_ptr()->get_bowl_type());
        DismType dism = sample_pf_map<DismType>(dism_pf);

        // Pick a fielder
        Player* fielder =
            man_field.select_catcher(bowl1->get_player_ptr(), dism);

        // TODO: Fix this
        striker->dismiss(dism, bowl1->get_player_ptr(), fielder);

        // Create a object for fall of wicket
        fow[wkts - 1] = {striker->get_player_ptr(), (unsigned int)wkts + 1,
                         (unsigned int)team_score, (unsigned int)overs,
                         (unsigned int)balls};

        // Print dismissal
        if (!is_quiet)
            std::cout << BUFFER + striker->print_card() << std::endl;

        // Update match time
        // t_output = time->delivery(false, runs);

        // Determine next batter
        if (wkts < 10) {
            striker = man_bat.next_in(this);
            striker->activate();

            if (!is_quiet) {
                std::cout << striker->get_player_ptr()->get_full_name()
                          << +" is the new batter to the crease" << std::endl;
            }

            // Create new partnership tracker
            bat_parts[wkts - 1]->end();
            bat_parts[wkts] = new Partnership(striker->get_player_ptr(),
                                              nonstriker->get_player_ptr());

        } // All out is checked immediately after with check_state

    } else {
        // Update score trackers
        int runs = outcome.front() - '0';
        team_score += runs;
        lead += runs;
        bat_parts[wkts]->add_runs(
            runs, bat_parts[wkts]->get_bat2() == striker->get_player_ptr(),
            is_legal);

        bool is_rotation;
        if (is_legal) {
            legal_delivs++;
            // Check for strike rotation
            is_rotation = runs % 2 == 1 && runs != 5;
        } else
            is_rotation = runs - 1 % 2 == 1;

        // Rotate strike if required
        if (is_rotation)
            swap_batters();

        // Update MatchTime
    }
}

// Check for declaration
bool Innings::check_declaration() {
    // TODO: implement declaration checking
    return false;
    // AIS: never declare, may lead to some slightly absurd innings
}

/* Possible return values:
 *  allout - batting team is bowled out
 *  won - batting team has succesfully chased a 4th innings target
 *  draw - reached end of match time
 *  dec - batting team has declared
 *
 **/
std::string Innings::check_state() {
    // Check for close of innings
    // Match object distinguishes different types of win
    if ((inns_no == 4 && lead > 0)) {
        // 4th innings chase
        is_open = false;
        return "win";
    }

    if (wkts == 10) {
        // Bowled out
        is_open = false;
        return "allout";
    }

    // Check for declaration
    if (check_declaration()) {
        is_open = false;
        return "dec";
    };

    // Check for draw

    // Check for end of day

    // Check for end of over
    if (last_over->get_num_legal_delivs() == 6) {
        end_over();
    }

    return "";
}

void Innings::end_over() {
    if (!is_quiet) {
        std::cout << DIVIDER << std::endl
                  << comm_over(last_over) << std::endl
                  << DIVIDER << std::endl;
    }

    overs++;

    // Apply rest to all bowlers
    for (int i = 0; i < 11; i++) {
        BowlerCard* curr = bowlers[i];
        if (curr != bowl1)
            curr->over_rest();
    }

    // Switch ends
    swap_batters();
    swap_bowlers();

    // Create a new over object
    Over* new_over = new Over(overs + 1);
    last_over->set_next(new_over);
    last_over = new_over;

    // Special case - second over
    if (overs == 1) {
        if (!is_quiet) {
            std::cout << "Opening from the other end is " +
                             bowl1->get_player_ptr()->get_full_name() + ".\n";
        }
    } else {
        // Consult the bowling manager
        BowlerCard* new_bc = man_bowl.end_over(this);

        if (!is_quiet && new_bc != bowl1) {
            std::cout << "Change of bowling, " +
                             new_bc->get_player_ptr()->get_full_name() +
                             " into the attack.\n";
        }
        bowl1 = new_bc;
    }
}

void Innings::swap_batters() {
    BatterCard* tmp = striker;
    striker = nonstriker;
    nonstriker = tmp;
}

void Innings::swap_bowlers() {
    BowlerCard* tmp = bowl1;
    bowl1 = bowl2;
    bowl2 = tmp;
}

void Innings::cleanup() {}

std::string Innings::comm_ball(int overs, Player* bowler, Player* batter,
                               std::string outcome) {
    int balls = last_over->get_num_legal_delivs();
    if (!(last_over->get_last()->legal)) {
        balls++;
    }

    std::string output = std::to_string(overs) + "." + std::to_string(balls) +
                         " " + bowler->get_last_name() + " to " +
                         batter->get_last_name() + ", ";

    if (outcome == "W") {
        output += "OUT!";
    } else {
        output += outcome;
        // TODO: improve this
    }

    return output;
}

std::string Innings::comm_over(Over* over) {
    // std::cout << over << std::endl;
    std::string output = "End of Over " + std::to_string(over->get_over_num()) +
                         BUFFER + BUFFER + BUFFER + team_bat->name + ": " +
                         score() + DIVIDER + "\n";

    // Batter bowler details
    output += striker->print_short() + BUFFER + bowl1->print_card() + "\n";
    output += nonstriker->print_short() + BUFFER + bowl2->print_card() + "\n";

    return output;
}

std::string Innings::score() {
    if (AUSTRALIAN_STYLE)
        return std::to_string(wkts) + "/" + std::to_string(team_score);
    else
        return std::to_string(team_score) + "/" + std::to_string(wkts);
}

std::string Innings::simulate(bool quiet) {
    is_quiet = quiet;

    if (!is_quiet) {
        // Pre-innings chatter
        std::cout
            << "Here come the teams...\n"
            << team_bowl->name + " lead by captain " +
                   (team_bowl->players[team_bowl->i_captain])->get_full_name()
            << ".\n"
            << bowl1->get_player_ptr()->get_full_name() +
                   " has the new ball in hand and is about to bowl to " +
                   striker->get_player_ptr()->get_full_name() + ".\n"
            << nonstriker->get_player_ptr()->get_full_name() +
                   " is at the non-strikers end.\n"
            << "Let's go!\n"
            << DIVIDER << std::endl;
    }

    std::string state;
    while (is_open) {
        // Simulate a single delivery
        simulate_delivery();

        // Check match state
        state = check_state();
    }

    if (!is_quiet) {
        // Print lead
        std::cout << team_bat->name << " ";
        if (lead > 0)
            std::cout << "lead by ";
        else
            std::cout << "trail by ";
        std::cout << std::to_string(abs(lead)) << " runs.\n";
    }

    cleanup();
    return state;
}

std::ostream& operator<<(std::ostream& os, const Innings& inns) {
    const int name_width = 20;
    const int dism_width = 30;

    // Header
    os << Innings::DIVIDER << inns.team_bat->name << " "
       << ordinal(inns.inns_no) << " Innings" << Innings::DIVIDER;

    // Titles
    print_spaced<std::string>(os, "Batter", name_width);
    print_spaced<std::string>(os, "", dism_width);
    print_spaced<std::string>(os, "R", 5);
    print_spaced<std::string>(os, "B", 5);
    print_spaced<std::string>(os, "4s", 5);
    print_spaced<std::string>(os, "6s", 5);
    print_spaced<std::string>(os, "SR", 5);

    os << Innings::DIVIDER;

    // Print each batter
    for (int i = 0; i < 11; i++) {
        BatterCard* ptr = inns.batters[i];
        BatStats stats = ptr->get_sim_stats();

        // Calculate strike rate
        std::string sr =
            print_rounded((float)stats.runs / stats.balls * 100, 2);

        // Player name - mark if captain or wicketkeeper
        std::string name = ptr->get_player_ptr()->get_full_initials();
        if (ptr->get_player_ptr() ==
            inns.team_bat->players[inns.team_bat->i_captain]) {
            name += " (c)";
        }
        if (ptr->get_player_ptr() ==
            inns.team_bat->players[inns.team_bat->i_wk]) {
            name += " (wk)";
        }

        if (ptr->is_active()) {
            print_spaced<std::string>(os, name, name_width);
            print_spaced<std::string>(os, ptr->print_dism(), dism_width);
            print_spaced<int>(os, stats.runs, 5);
            print_spaced<int>(os, stats.balls, 5);
            print_spaced<int>(os, stats.fours, 5);
            print_spaced<int>(os, stats.sixes, 5);
            print_spaced<std::string>(os, sr, 5);

            os << std::endl;
        }
    }
    // Extras
    os << Innings::DIVIDER;
    print_spaced<std::string>(os, "Extras", name_width);
    print_spaced<std::string>(os, "(" + inns.extras.print() + ")", dism_width);
    print_spaced<int>(os, inns.extras.total(), 5);
    os << Innings::DIVIDER;

    // Total score
    int over_balls = inns.last_over->get_num_legal_delivs();
    std::string rr = print_rounded(
        inns.team_score / (inns.overs + (float)over_balls / 6.0), 2);

    print_spaced<std::string>(os, "Total", name_width);
    print_spaced<std::string>(
        os, "(" + std::to_string(inns.overs) + " Ov, RR " + rr + ")",
        dism_width);
    os << inns.team_score;

    if (inns.wkts < 10) {
        os << "/" << inns.wkts;
        if (false) {
            // TODO: Use declared flag
            os << "d";
        }

        os << Innings::DIVIDER;

        // Did not bat
        os << "Did not bat: ";
    }

    os << Innings::DIVIDER;

    // fall of wickets
    os << "Fall of Wickets: ";

    os << Innings::DIVIDER;

    // Bowlers
    print_spaced<std::string>(os, "Bowling", name_width);
    print_spaced<std::string>(os, "O", 6);
    print_spaced<std::string>(os, "M", 5);
    print_spaced<std::string>(os, "R", 5);
    print_spaced<std::string>(os, "W", 5);
    print_spaced<std::string>(os, "Econ", 5);
    os << Innings::DIVIDER;

    for (int i = 0; i < 11; i++) {
        // Calculate and format economy
        BowlerCard* ptr = inns.bowlers[i];
        BowlStats stats = ptr->get_sim_stats();

        // Only print if they have bowled a ball
        if (stats.balls > 0) {
            std::pair<int, int> overs = balls_to_ov(stats.balls);

            // Calculate economy
            std::string econ =
                print_rounded(stats.runs / (overs.first + overs.second / 6.0));

            print_spaced<std::string>(
                os, ptr->get_player_ptr()->get_full_initials(), name_width);

            std::string over_str = std::to_string(overs.first);
            if (overs.second > 0)
                over_str += "." + std::to_string(overs.second);

            print_spaced<std::string>(os, over_str, 6);
            print_spaced<int>(os, stats.maidens, 5);
            print_spaced<int>(os, stats.runs, 5);
            print_spaced<int>(os, stats.wickets, 5);
            print_spaced<std::string>(os, econ, 5);
            os << std::endl;
        }
    }

    return os;
}

// Getters
BatterCard** Innings::get_batters() { return batters; }

BowlerCard** Innings::get_bowlers() { return bowlers; }

bool Innings::get_is_open() { return is_open; }

int Innings::get_lead() { return lead; }

int Innings::get_wkts() { return wkts; }

Team* Innings::get_bat_team() { return team_bat; }

Team* Innings::get_bowl_team() { return team_bowl; }

// Destructor
Innings::~Innings() {
    // Delete each dynamically allocated BatterCard and BowlerCard
    for (int i = 0; i < 11; i++) {
        delete batters[i], bowlers[i];
    }

    delete[] temp_outcomes;
    delete[] fow;

    // Delete each over iteratively
    delete_linkedlist<Over>(first_over);

    // Delete each partnership
    for (int i = 0; i < 10; i++) {
        if (bat_parts[i] != nullptr)
            delete bat_parts[i];
    }
}

/*
  Match implementations
*/
Match::Match(Pregame detail)
    : team1(detail.home_team), team2(detail.away_team), venue(detail.venue),
      ready(false), inns_i(0) {

    // Time object - default constructor to day 1, start time
    // time = MatchTime();
}

void Match::simulate_toss() {
    Team* winner;
    Team* loser;
    TossChoice choice;

    // Winner of toss is chosen randomly - 0.5 probability either way
    if ((double)rand() / (RAND_MAX) < 0.5) {
        winner = team1;
        loser = team2;
    } else {
        winner = team1;
        loser = team2;
    }

    if ((double)rand() / (RAND_MAX) <
        prediction::toss_elect(venue->pitch_factors->spin)) {
        choice = field;
    } else {
        choice = bat;
    }

    toss = {winner, loser, choice};

    // Print toss result
    std::cout << toss_str() << std::endl;
}

void Match::change_innings() {
    Team *new_bat, *new_bowl;

    if (inns_i == 1 && DECIDE_FOLLOW_ON(-lead)) {
        // Follow on
        new_bat = inns[inns_i]->get_bat_team();
        new_bowl = inns[inns_i]->get_bowl_team();
    } else {
        // Standard swap
        lead *= -1;
        new_bat = inns[inns_i]->get_bowl_team();
        new_bowl = inns[inns_i]->get_bat_team();
    }

    inns_i++;
    inns[inns_i] = new Innings(new_bat, new_bowl, lead, venue->pitch_factors);
}

/**
 * @brief Decide whether to enforce the follow-on, based on the lead
 *
 * Decision is made randomly, using a probability given by MODEL_FOLLOW_ON.
 */
bool Match::DECIDE_FOLLOW_ON(int lead) {
    if (lead < 200)
        return false;
    else {
        // Use model to randomly decide whether or not to enforce the follow-on
        double r = prediction::follow_on(lead);
        return (double)rand() / (RAND_MAX) < r;
    }
}

std::string Match::toss_str() { return std::string(toss); }

void Match::pregame() {
    // Toss
    simulate_toss();

    // Set up Innings object
    if (toss.choice == bat)
        inns[0] = new Innings(toss.winner, toss.loser, 0, venue->pitch_factors);
    else if (toss.choice == field)
        inns[0] = new Innings(toss.loser, toss.winner, 0, venue->pitch_factors);
    else
        // Throw exception
        throw(std::invalid_argument("Undefined TossChoice value."));

    ready = true;
}

void Match::start(bool quiet) {
    std::string inns_state;

    while (inns_i < 4) {
        inns_state = inns[inns_i]->simulate(quiet);
        lead = inns[inns_i]->get_lead();

        if (!quiet) {
            // Print lead
            std::cout << inns[inns_i]->get_bat_team()->name << " ";
            if (lead > 0)
                std::cout << "lead by ";
            else
                std::cout << "trail by ";
            std::cout << std::to_string(abs(lead)) << " runs.\n";
        }

        // Determine if game has been won
        if (inns_i == 2 && inns_state == "allout" && lead < 0) {
            // Win by innings
            result = new MatchResult(win_innings, inns[inns_i]->get_bowl_team(),
                                     -lead);
            break;

        } else if (inns_i == 3) {
            // 4th innings scenarios
            if (inns_state == "allout") {

                if (lead == 0) {
                    // Tie
                    result = new MatchResult(tie);
                } else {
                    // Bowled out
                    result = new MatchResult(
                        win_bowling, inns[inns_i]->get_bowl_team(), -lead);
                }

            } else if (inns_state == "win") {
                // Win chasing
                result =
                    new MatchResult(win_chasing, inns[inns_i]->get_bat_team(),
                                    10 - inns[inns_i]->get_wkts());

            } else if (inns_state == "draw") {
                // Draw
                result = new MatchResult(draw);
            } else {
                // Raise an exception, Innings::simulate() has returned
                // something unknown
            }
            break;
        } else {
            // Change innings
            change_innings();
        }
    }
}

void Match::print_all() {
    for (int i = 0; i < 4; i++) {
        if (inns[i] != nullptr)
            std::cout << *inns[i] << std::endl;
    }

    std::cout << result->print() << std::endl;
}

Match::~Match() {
    // Delete each innings
    for (int i = 0; i < 4; i++) {
        delete inns[i];
    }

    delete result;
}