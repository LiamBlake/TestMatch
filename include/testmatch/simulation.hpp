// -*- lsst-c++ -*-
/* simulation.hpp
 *
 *
 */

#ifndef SIMULATION_H
#define SIMULATION_H

#include "cards.hpp"
#include "enums.hpp"
#include "matchtime.hpp"
#include "models.hpp"
#include "pregame.hpp"
#include "team.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// Forward declaration allows for referencing Innings object in managers
class Innings;

// Match details required in delivery model
struct MatchStats {};

/**
 * @brief Manages batting order by passing BatterCard pointers to Innings
 *
 *
 */
class BattingManager {
  private:
    BatterCard* cards[11];
    bool batted[11];

    // Various options for determining next batter
    BatterCard* next_ordered();
    BatterCard* nightwatch();
    BatterCard* promote_hitter();

  public:
    // Constructor
    BattingManager();

    /**
     * @brief
     * @param c_cards
     */
    void set_cards(BatterCard** c_cards);

    /**
     * @brief
     * @param inns_obj Pointer to Innings object, used to get game situation
     * @return Pointer to BatterCard corresponding to the new batter
     */
    BatterCard* next_in(Innings* inns_obj);
};

/**
 * @brief
 */
class BowlingManager {
  private:
    BowlerCard* cards[11];

    int n_over_calls;

    /**
     * @brief
     * @param bowl_avg
     * @param bowl_sr
     * @param fatigue
     * @return
     */
    static double bowler_obj(double bowl_avg, double bowl_sr, double fatigue);

    /**
     * @brief
     * @param fatigue
     * @return
     */
    static double take_off_prob(double fatigue);

    /**
     * @brief Various options for getting a new bowler
     * @param ignore1
     * @param ignore2
     * @return A BowlerCard pointer to the new bowler
     */
    BowlerCard* new_pacer(BowlerCard* ignore1, BowlerCard* ignore2);
    BowlerCard* new_spinner(BowlerCard* ignore1, BowlerCard* ignore2);
    BowlerCard* part_timer(BowlerCard* ignore1, BowlerCard* ignore2);
    BowlerCard* change_it_up(BowlerCard* ignore1, BowlerCard* ignore2);
    BowlerCard* any_fulltime(BowlerCard* ignore1, BowlerCard* ignore2);

    /**
     * @brief
     *
     * Note: This is defined here, rather than Simulation.cpp, because the
     * function is a template.
     *
     * @tparam pred
     * @param predicate Lambda function taking a BowlerCard* and returning a
     * boolean indicating whether to consider that bowler when searching.
     * @return BowlerCard* Pointer to the BowlerCard of the chosen bowler.
     */
    template <class pred>
    BowlerCard* search_best(pred predicate) {
        double min_obj = std::numeric_limits<double>::max();
        double new_obj;
        BowlerCard* best = nullptr;
        BowlerCard* curr;
        Player* curr_ply;
        for (int i = 0; i < 11; i++) {
            curr = cards[i];
            curr_ply = curr->get_player_ptr();

            // Only consider if pace bowler and full-time
            if (predicate(curr)) {
                // Calculate objective function
                new_obj = Model::OBJ_AVG_FATIG(curr_ply->get_bowl_avg(),
                                               curr_ply->get_bowl_sr(),
                                               curr->get_tiredness());

                // Compare to current best
                if (new_obj < min_obj) {
                    best = curr;
                    min_obj = new_obj;
                }
            }
        }

        // Return null if no such bowler can be found
        return best;
    }

  public:
    BowlingManager();

    void set_cards(BowlerCard* c_cards[11]);

    /**
     * @brief
     * @param inns_obj Pointer to Innings object, used to get game situation
     * @return Pointer to BowlerCard corresponding to the bowler bowling the
     * next over
     */
    BowlerCard* end_over(Innings* inns_obj);
};

/**
 * @brief
 */
class FieldingManager {
  private:
    static double C_WK_PROB;

    Player* players[11];
    int wk_idx;

  public:
    FieldingManager(int c_wk_idx);

    void set_cards(Player* c_plys[11]);
    /**
     * @brief Select a fielder for an appropriate mode of dismissial
     * @param bowler Pointer to the bowler Player object
     * @param run_out Logical indicating whether the dismissal is a runout,
     * default false
     * @return
     */
    Player* select_catcher(Player* bowler, DismType dism_type);
};

/**
 * @brief
 */
class Innings {
  private:
    // Each team
    Team* team_bat;
    Team* team_bowl;

    // General innings info
    static int NO_INNS;
    int inns_no;
    static double PRINT_DELAY;
    bool is_quiet;

    int overs;
    int balls;
    int legal_delivs;
    int team_score;
    int lead;
    int wkts;

    bool is_open;

    // MatchTime* time;
    PitchFactors* pitch;

    // Ball-by-ball detail
    Over* first_over;
    Over* last_over;

    // Scorecards
    BatterCard** batters;
    BowlerCard** bowlers;

    // Managers
    BattingManager man_bat;
    BowlingManager man_bowl;
    FieldingManager man_field;

    // Current batters
    BatterCard* striker;
    BatterCard* nonstriker;

    // Bowler of current over (bowl1) and previous over (bowl2)
    BowlerCard* bowl1;
    BowlerCard* bowl2;

    // Partnerships
    Partnership* bat_parts[10];

    // Extras and fall-of-wicket
    Extras extras;
    FOW* fow;

    std::string* temp_outcomes;

    // Private methods used in simulation process

    // Simulate a delivery and update appropriate statistics
    void simulate_delivery();

    // Called after each delivery, checks for changes in game state, such as end
    // of over, end of innings, declaration, scheduled break, etc.
    std::string check_state();

    // Check for declaration
    bool check_declaration();

    // Handle end of over
    void end_over();

    /**
     * @brief Functions for swapping batter and bowler pointers respectively
     */
    void swap_batters();
    void swap_bowlers();

    /**
     * @brief
     * @param outcome
     * @return
     */
    std::string comm_ball(int overs, Player* bowler, Player* batter,
                          std::string outcome);

    /**
     * @brief
     * @return
     */
    std::string comm_over(Over* over);

    // Whether to use Australia style of scoring, wickets/runs, or the
    // international runs/wickets
    static bool AUSTRALIAN_STYLE;
    std::string score();

    std::string print_fow();

    /**
     * @brief Called when innings closed, sets all batters to inactive, etc.
     */
    void cleanup();

    static std::string DIVIDER;
    static std::string BUFFER;

  public:
    // Constructor
    Innings(Team* c_team_bat, Team* c_team_bowl, int c_lead,
            PitchFactors* c_pitch); // MatchTime* c_time);

    // Returns state string explainining why innings has ended
    std::string simulate(bool quiet = true);

    friend std::ostream& operator<<(std::ostream& os, const Innings& inns);

    // Getters
    BatterCard** get_batters();
    BowlerCard** get_bowlers();

    bool get_is_open();
    int get_lead();
    int get_wkts();
    Team* get_bat_team();
    Team* get_bowl_team();

    // Destructor
    ~Innings();

    // Allow manager objects to access private members
    friend class BattingManager;
    friend class BowlingManager;
    friend class FieldingManager;
};

/**
 * @brief
 */
class Match {

  private:
    Team* team1; // Home team
    Team* team2; // Away team

    Venue* venue;

    bool ready;
    TossResult toss;

    // MatchTime time;
    std::string match_state;

    // Tracking current game state
    int inns_i;
    Innings* inns[4];
    int lead;
    int match_balls;

    // Storing winner detail
    MatchResult* result;

    // Private helper functions

    /**
     * @brief
     */
    void simulate_toss();

    /**
     * @brief
     */
    void change_innings();

    /**
     * @brief Decide whether to enforce the follow-on, based on the lead.
     *
     * Wrapper for Match::MODEL_FOLLOW_ON
     *
     * @param lead Lead of bowling team at end of previous innings.
     * @return Boolean indicating whether the follow-on is enforced
     */
    static bool DECIDE_FOLLOW_ON(int lead);

    // For printing
    /**
     * @brief Returns a string detailing which team won the toss and the choice
     * @return Aforementioned string
     */
    std::string toss_str();

    std::string winner_str();

  public:
    Match(Pregame detail);

    /**
     * @brief
     */
    void pregame();

    /**
     * @brief
     * @param quiet
     */
    void start(bool quiet = true);

    /**
     * @brief 
     * 
     */
    void print_all();

    ~Match();
};

#endif // SIMULATION_h
