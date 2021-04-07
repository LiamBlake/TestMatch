// -*- lsst-c++ -*-
/* MatchTime.h
 *
 *
 */

#ifndef MATCHTIME_H
#define MATCHTIME_H

#include <iostream>
#include <string>
#include <utility>

// For brevity of code
typedef unsigned int uint;

class TimeOfDay {
  private:
    uint _sec;
    uint _min;
    uint _hrs;

    static std::string two_digits(int val);
    void round();

  public:
    TimeOfDay();
    TimeOfDay(uint hrs, uint min, uint sec);
    TimeOfDay(double dr);

    // Explicit casts
    operator std::string();
    operator int();
    operator double();

    // Getters
    uint sec();
    uint min();
    uint hrs();

    // Explicit setter
    void set(double dr);

    // Overloaded operators
    TimeOfDay& operator++();
    TimeOfDay operator++(int);

    TimeOfDay& operator+=(const TimeOfDay& rhs);
    TimeOfDay& operator+=(const int& rhs);

    friend bool operator==(const TimeOfDay& lhs, const TimeOfDay& rhs);

    friend TimeOfDay operator+(TimeOfDay lhs, const TimeOfDay& rhs);
    friend TimeOfDay operator+(TimeOfDay lhs, const int& rhs);
};

class MatchTime {

  private:
    // Parameters
    static float START_TIME;
    static float LUNCH_START;
    static int LUNCH_DUR; // 40 minutes
    static float TEA_START;
    static int TEA_DUR; // 20 minutes
    static float CLOSE_PLAY;

    static int OVERCHA_DUR;
    static int DRINKS_DUR; // 5 minutes
    static int INNBRE_DUR; // 10 minutes

    // Maximum amount of time which a session can be extended by
    static int MAXIMUM_EXTEND; // 30 minutes

    // Delivery duration statistics
    static double PACE_MINDUR;
    static double PACE_MAXDUR;
    static double SPIN_MINDUR;
    static double SPIN_MAXDUR;
    static double PACE_MEANDUR;
    static double SPIN_MEANDUR;
    // static double DEL_STDDUR = 0.5;
    static double RUN_DUR;

    TimeOfDay time;
    int day;

    std::string state;
    /* Permissable match states
        "Match Start"   - Time at start of session 1, day 1
        "Session 1"     - Session 1, as defined in Parameters.h
        "Drinks 1"      - Drinks break during session 1
        "Lunch"         - Lunch break, as defined in Parameters.h
        "Session 2"     - As above
        "Drinks 2"      - As above
        "Tea"           - Tea break, as defined in Parameters.h
        "Session 3"     - As above
        "Drinks 3"      - As above
        "Innings Break" - Break between team innings
        "Stumps"        - End of day
        "Rain Delay"    - Currently unused
    */

    // Indicates whether current session has been extended by 30 minutes
    bool extended;

    // Indicates whether drinks have been taken in the current session
    bool after_drinks;

    // Private utility functions
    bool take_lunch();
    bool take_tea();
    bool close_day();
    void check_state_change();

  public:
    // Default constructor - start of match, day 1
    MatchTime();
    // MatchTime(Time c_tm, int c_day, std::string c_state);

    // Time controls for use by simulation
    std::pair<int, std::string> delivery(bool type, int runs);
    std::pair<int, std::string> wicket();
    std::pair<int, std::string> end_over();
    std::pair<int, std::string> drinks();
    std::string force_early_break();
    std::string extend_session();
    std::string end_innings();
    // Returned strings indicate match state, used for error checking and
    // interaction with Match object

    // Getters
    TimeOfDay get_time();
    int get_day();
    std::string get_state();

    // Overloaded operators
    operator std::string();
};

#endif // MATCHTIME_H