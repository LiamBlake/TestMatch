#include "testmatch/matchtime.hpp"

#include "testmatch/helpers.hpp"

#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

typedef unsigned int uint;

/*
    Time implementations
*/

std::string TimeOfDay::two_digits(int val) {
    std::string output;

    if (val < 10)
        output += "0";
    output += std::to_string(val);
    return output;
}

TimeOfDay::TimeOfDay(uint c_hour, uint c_min, u_int c_sec)
    : hour(c_hour), min(c_min), sec(c_sec) {}

TimeOfDay::TimeOfDay(float val) { set(val); }

void TimeOfDay::set(uint n_hour, uint n_min, u_int n_sec) {
    hour = n_hour;
    min = n_min;
    sec = n_sec;
}

void TimeOfDay::set(float val) {
    sec = 0;
    hour = val;
    min = std::round(val * 100) - int(val) * 100;

    // TODO: Check for invalid values
}

uint TimeOfDay::get_hour() { return hour; }
uint TimeOfDay::get_min() { return min; }
uint TimeOfDay::get_sec() { return sec; }

TimeOfDay& TimeOfDay::operator+=(const int& rhs) {
    sec += rhs;

    if (sec > 60) {
        min += sec / 60;
        sec = sec % 60;

        if (min > 60) {
            hour = (hour + min / 60) % 24;
            min = min % 60;
        }
    }

    return *this;
}

TimeOfDay::operator std::string() {
    std::string am_pm_str;
    if (hour < 12) {
        am_pm_str = "am";
    } else {
        am_pm_str = "pm";
    }

    uint hour_12;
    if (hour == 0 || hour == 12) {
        hour_12 = 12;
    } else {
        hour_12 = hour % 12;
    }

    return (std::to_string(hour_12) + ":" + two_digits(min) + " " + am_pm_str);
}

bool operator==(const TimeOfDay lhs, const TimeOfDay& rhs) {
    return (lhs.hour == rhs.hour) && (lhs.min == rhs.min) &&
           (lhs.sec == rhs.sec);
}

bool operator==(const TimeOfDay lhs, const float& rhs) {
    TimeOfDay tod_rhs(rhs);
    return lhs == tod_rhs;
}

int operator-(const TimeOfDay lhs, const TimeOfDay& rhs) {
    return 3600 * (lhs.hour - rhs.hour) + 60 * (lhs.min - rhs.min) +
           (lhs.sec - rhs.sec);
}

int operator-(const TimeOfDay lhs, const float& rhs) {
    TimeOfDay tod_rhs(rhs);
    return lhs - tod_rhs;
}

int operator-(const float lhs, const TimeOfDay& rhs) { return -(rhs - lhs); }

bool operator>(const TimeOfDay lhs, const TimeOfDay& rhs) {
    if (lhs.hour > rhs.hour) {
        return true;
    } else if (lhs.hour < rhs.hour) {
        return false;
    }

    if (lhs.min > rhs.min) {
        return true;
    } else if (lhs.min < rhs.min) {
        return false;
    }

    return lhs.sec > rhs.sec;
}

bool operator>(const TimeOfDay lhs, const float& rhs) {
    TimeOfDay tod_rhs(rhs);
    return lhs > tod_rhs;
}

bool operator>=(const TimeOfDay lhs, const TimeOfDay& rhs) {
    return (lhs > rhs) || (lhs == rhs);
}

bool operator>=(const TimeOfDay lhs, const float& rhs) {
    return (lhs > rhs) || (lhs == rhs);
}

/*
    MatchTime implementations
*/
// Parameters
// Session time details
float MatchTime::START_TIME = 10.30;
float MatchTime::LUNCH_START = 12.30;
int MatchTime::LUNCH_DUR = 2400; // 40 minutes
float MatchTime::TEA_START = 15.10;
int MatchTime::TEA_DUR = 1200; // 20 minutes
float MatchTime::CLOSE_PLAY = 17.30;

int MatchTime::OVERCHA_DUR = 30;
int MatchTime::DRINKS_DUR = 300; // 5 minutes
int MatchTime::INNBRE_DUR = 600; // 10 minutes

// Maximum amount of time which a session can be extended by
int MatchTime::MAXIMUM_EXTEND = 1800; // 30 minutes

// Delivery duration statistics
double MatchTime::PACE_MINDUR = 25;
double MatchTime::PACE_MAXDUR = 60;
double MatchTime::SPIN_MINDUR = 15;
double MatchTime::SPIN_MAXDUR = 40;
double MatchTime::PACE_MEANDUR = 45;
double MatchTime::SPIN_MEANDUR = 30;
// const double DEL_STDDUR = 0.5;
double MatchTime::RUN_DUR = 10;

// Default constructor - start of match, day 1
MatchTime::MatchTime() : time(START_TIME) {
    day = 1;
    state = "Match Start";
}

// Private utility functions
bool MatchTime::take_lunch() {
    if (state != "Session 1") {
        // Cannot take lunch if not in Session 1
        return false;
    }

    // Push time to end of break
    time += LUNCH_DUR;
    state = "Lunch";
}

bool MatchTime::take_tea() {
    if (state != "Session 2") {
        // Cannot take tea if not in Session 2
        return false;
    }

    // Push time to end of break
    time += TEA_DUR;
    state = "Tea";
}

bool MatchTime::close_day() {
    if (state != "Session 3") {
        // Cannot close if not in Session 3
        return false;
    }

    // Push time to end of break
    day += 1;
    time.set(START_TIME);
    state = "Stumps";
}

void MatchTime::check_state_change() {

    // Switch through each possible match state
    if (state == "Match Start") {
        // if (time > START_TIME)
    } else if (state == "Session 1") {

    } else if (state == "Drinks 1") {

    } else if (state == "Lunch") {

    } else if (state == "Session 2") {

    } else if (state == "Drinks 2") {

    } else if (state == "Tea") {

    } else if (state == "Session 3") {

    } else if (state == "Drinks 3") {

    } else if (state == "Innings Break") {

    } else if (state == "Stumps") {

    } else {
        // Raise exception - unknown match state
    }
}

// Time controls for use by simulation
std::pair<int, std::string> MatchTime::delivery(bool type, int runs) {

    // Randomnly generate time elapsed by delivery
    double s;
    if (type) {
        // Spin bowler
        s = rtexp(SPIN_MEANDUR, SPIN_MINDUR, SPIN_MAXDUR) + runs * RUN_DUR;
    } else {
        // Pace bowler
        s = rtexp(PACE_MEANDUR, PACE_MINDUR, PACE_MAXDUR) + runs * RUN_DUR;
    }

    int elapsed = (int)round(s);

    check_state_change();
    std::pair<int, std::string> output = {elapsed, state};
    return output;
}

std::pair<int, std::string> MatchTime::end_over() { return {0, state}; }

std::pair<int, std::string> MatchTime::drinks() {
    time += DRINKS_DUR;

    check_state_change();
    return {DRINKS_DUR, state};
}

std::string MatchTime::force_early_break() {
    check_state_change();
    return state;
}

std::string MatchTime::extend_session() {
    check_state_change();
    return state;
}

// Getters
TimeOfDay MatchTime::get_time() { return time; }

int MatchTime::get_day() { return day; }

std::string MatchTime::get_state() { return state; }
