// -*- lsst-c++ -*-

#ifndef CARDS_H
#define CARDS_H

#include "enums.hpp"
#include "team.hpp"

#include <boost/serialization/base_object.hpp>
#include <random>
#include <string>

// Global Parameters
const double PACE_MEAN_FATIGUE = 0.1;
const double SPIN_MEAN_FATIGUE = 0.04;

/**
 * @brief Storage for batting statistics for simulation
 *
 *
 */
struct BatStats {

    // Career averages
    double bat_avg;
    double strike_rate;

    // Batting hand
    Arm bat_arm;

    // Current innings
    int runs;
    int balls;
    int fours;
    int sixes;
};

/**
 * @brief Storage for bowling statistics for simulation
 *
 *
 */
struct BowlStats {
    // Career averages
    double bowl_avg;
    double strike_rate;

    // Bowling type
    Arm bowl_arm;
    BowlType bowl_type;

    // Current innings
    int balls;
    int overs;
    int over_balls;
    int maidens;
    int runs;
    int wickets;
    int legal_balls;

    int spell_balls;
    double spell_overs;
    int spell_maidens;
    int spell_runs;
    int spell_wickets;
};

/**
 * @brief Storage of dismissal details
 */
class Dismissal {
  private:
    DismType mode;
    Player* bowler;
    Player* fielder;

    // if bowled or lbw, set fielder = NULL

  public:
    // Constructors
    Dismissal(){};
    /**
     * @brief
     * @param c_mode
     * @param c_bowler
     * @param c_fielder
     */
    Dismissal(DismType c_mode, Player* c_bowler = nullptr,
              Player* c_fielder = nullptr);

    /**
     * @brief
     * @return
     */
    std::string print_dism();

    /**
     * @brief
     * @return
     */
    DismType get_mode();

    /**
     * @brief
     * @return
     */
    Player* get_bowler();

    /**
     * @brief
     * @return
     */
    Player* get_fielder();
};

/**
 * @brief Measure of tiredness of bowler, model for determining next bowler
 *
 *
 */
class Fatigue {
  private:
    double value;

    // Normal distribution object for generating values
    static std::random_device RD;
    static std::mt19937 GEN;
    std::normal_distribution<double>* dist;

    // Parameters
    static double MEAN_PACE_FATIGUE;
    static double MEAN_SPIN_FATIGUE;
    static double EXTRA_PACE_PENALTY;
    static double VAR_PACE_FATIGUE;
    static double VAR_SPIN_FATIGUE;

  public:
    // Constructor
    Fatigue(){};
    Fatigue(BowlType c_bowl_type);

    // Getter
    double get_value();

    // Events which change fatigue
    void ball_bowled();
    void wicket();
    void rest(double time);

    // Destructor
    ~Fatigue();
};

/**
 * @brief Abstract base class for bowler and batter cards.
 */
class PlayerCard {

  protected:
    Player* player;
    int order;
    bool activated;

  public:
    // Constructors
    PlayerCard(){};
    PlayerCard(Player* c_player, int c_order);

    // Getters
    Player* get_player_ptr();
    int get_order();

    // Pure virtual methods
    virtual void update_score(std::string outcome) = 0;
    virtual std::string print_card(void) = 0;

    // Default destructor
};

/**
 * @brief
 */
class BatterCard : public PlayerCard {

  private:
    BatStats stats;
    bool active;
    bool out;
    Dismissal* dism;

    int mins;

    //
    int playstyle_flag = 0;

  public:
    BatterCard() : PlayerCard(){};
    BatterCard(Player* c_player, int c_order);

    BatStats get_sim_stats(void);

    bool is_active(void);

    void activate(void);
    void update_score(std::string outcome); //, float mins);
    void dismiss(DismType d_mode, Player* d_bowler = nullptr,
                 Player* d_fielder = nullptr);
    std::string print_card(void);
    std::string print_short(void);
    std::string print_dism(void);

    // Copy constructor
    // BatterCard(const BatterCard& bc);

    ~BatterCard();
};

/**
 * @brief Storage as all information relevant to a bowler bowling in an innings
 */
class BowlerCard : public PlayerCard {

  private:
    BowlStats stats;
    bool active;

    // Flag indicating whether bowler is considered "part-time"
    int competency; // 0 - full-time bowler, 1 - part-time bowler, 2 - only bowl
                    // this player when the opposition is 2/700

    // Rudimentary measure of bowler tiredness, improves with each passing over
    Fatigue tiredness;

    // Tracks number of runs in a current over to determine whether that over
    // was a maiden
    bool is_maiden = true;

    void add_ball();

    // Determine whether a given player is considered a "parttime bowler"
    static int DETERMINE_COMPETENCY(Player* player);

  public:
    BowlerCard() : PlayerCard(){};
    BowlerCard(Player* c_player, int c_order);

    BowlStats get_sim_stats(void);
    void update_score(std::string outcome);
    void start_new_spell();

    // Get fatigue
    double get_tiredness();
    int get_competency();

    // Over passes (from same end) without bowler bowling - reduces fatigue
    void over_rest();

    std::string print_card(void);
    std::string print_spell(void);
};

template <typename T>
PlayerCard** sort_array(PlayerCard** list, int len,
                        T (Player::*sort_val)() const);

BatterCard** create_batting_cards(Team* team);
BowlerCard** create_bowling_cards(Team* team);

/**
 * @brief Storage for all information describing a single delivery
 *
 * Stores all information describing a single delivery, including pointers to
 * the Player objects storing the bowler and batter, the encoded outcome,
 * whether the delivery was legal (i.e. not a no ball, wide) and a (currently
 * unused) string containing commentary describing the ball. The Ball struct
 * also functions as a node in the linked list implementation of an Over,
 * storing a reference to the next Ball object in the over. This pointer is null
 * by default.
 */
struct Ball {
    Player* bowler;
    Player* batter;

    std::string outcome;
    bool legal;
    std::string commentary;

    // For linked list implementation
    Ball* next = nullptr;

    Ball* get_next() { return next; };
};

/**
 * @brief
 *
 * ... Functions as BOTH a linked list (with Ball objects as nodes), and nodes
 * in a larger linked list managed by the Innings object. Accordingly contains a
 * pointer to the next Over object in the innings, which is null by default.
 */
class Over {
  private:
    int over_num;

    Ball* first;
    Ball* last;

    int num_balls;
    int num_legal_delivs;

    Over* next = nullptr;

  public:
    // Iterators?

    // Constructor
    Over(){};
    Over(int c_over_num);

    /**
     * @brief
     * @param p_next
     */
    void set_next(Over* p_next);

    int get_over_num();
    int get_num_balls();
    int get_num_legal_delivs();
    Ball* get_first();
    Ball* get_last();
    Over* get_next();

    /**
     * @brief Add a Ball object to the end of the over
     * @param ball Pointer to the new Ball object
     */
    void add_ball(Ball* ball);

    ~Over();
};

class Extras {
  private:
    unsigned int byes;
    unsigned int legbyes;
    unsigned int noballs;
    unsigned int wides;

  public:
    Extras();

    bool update_score(std::string outcome);
    std::string print() const;
    int total() const;
};

struct FOW {

    Player* batter;
    unsigned int wkts;
    unsigned int runs;
    unsigned int overs;
    unsigned int balls;

    std::string print();

    // TODO: implement value checking for 0 <= balls < 6
};

/**
 * @brief
 *
 */
class Partnership {
  private:
    Player* bat1;
    Player* bat2;

    unsigned int runs;

    unsigned int bat1_runs;
    unsigned int bat1_balls;

    unsigned int bat2_runs;
    unsigned int bat2_balls;

    bool not_out;

  public:
    Partnership(){};
    Partnership(Player* c_bat1, Player* c_bat2);

    // Getters
    Player* get_bat1();
    Player* get_bat2();

    unsigned int get_runs();
    unsigned int get_balls();

    bool get_not_out();

    /**
     * @brief
     *
     * @param n_runs
     * @param scorer
     * @param add_ball
     * @return Milestone*
     */
    void add_runs(unsigned int n_runs, bool scorer, bool add_ball);
    void end();
};

/**
 * @brief Summarise the result of a match, including the winner and margin of
 * victory if applicable.
 *
 * There are five possible match results: a draw, .
 * This class is immutable, as there would be no reason to modify the values
 * after construction.
 */
class MatchResult {
  private:
    // Private constructor ensures values must be set upon construction
    MatchResult();

    /**
     * @brief Result type, as a ResultType enumeration. Possible values are
     * draw, win_chasing, win_bowling, win_innings and tie.
     *
     */
    ResultType type;
    /**
     * @brief Pointer to the Team object corresponding to the winning team. The
     * default value of nullptr should be set if a winner is not applicable
     * (e.g. a tie or draw). Value is access via the get_type() method.
     *
     */
    Team* winner;
    /**
     * @brief The margin of victory, if relevant. When the bowling team has won,
     * this is the number of runs the batting team were still trailing by upon
     * being bowled out. If the batting team has won, this is the number of
     * wickets remaining. The default value of 0 should be set if the margin is
     * not applicable (e.g. a tie or draw). Value is access via the get_margin()
     * method.
     *
     */
    unsigned int margin;

  public:
    /**
     * @brief Construct a new Match Result object.
     *
     * @param c_type The type of victory, as a ResultType enumeration.
     * @param c_winner Pointer to the winning team. This argument will be
     * ignored if a winning team is not applicable.
     * @param c_margin Winning margin. This argument will be ignored if a
     * winning team is not applicable.
     */
    MatchResult(ResultType c_type, Team* c_winner = nullptr,
                unsigned int c_margin = 0);

    // Getters
    ResultType get_type();
    Team* get_winner();
    unsigned int get_margin();

    /**
     * @brief Format the result type, winner and margin of victory in a nice,
     * printable string. For example, if India have won by 55 runs, the output
     * would be "India won by 55 runs".
     *
     * @return std::string The formatted string.
     */
    std::string print();
};

///// CURRENTLY UNDEFINED - FOR TRACKING MILESTONES

class Milestone {
  private:
    Player* player;
    int value;

    /* Pure virtual method - checks if passed value is
       allowed for given milestone type.
    */
    virtual bool is_permitted(int value) = 0;

  protected:
    std::string desc;

  public:
    Milestone(Player* c_player, int c_value);

    // Getters
    Player* get_player();
    std::string get_desc();
    int get_value();

    virtual std::string string() = 0;
};

class BatMilestone : public Milestone {
  private:
    bool is_permitted(int value);

    int runs;
    int balls;
    int fours;
    int sixes;

  public:
    BatMilestone(Player* batter, int c_milestone, int c_runs, int c_balls,
                 int c_fours, int c_sixes);

    std::string string();
};

class PartnershipMilestone : public BatMilestone {};

class BowlMilestone : public Milestone {
  private:
    bool is_permitted(int value);

    int runs;
    int overs;
    int balls;
    int maidens;

  public:
    BowlMilestone(Player* bowler, int c_milestone, int c_runs, int c_overs,
                  int c_balls, int c_maidens);

    std::string string();
};

#endif // CARDS_H