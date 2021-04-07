#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

#include "MatchTime.h"
#include "Utility.h"

TimeOfDay::TimeOfDay() : _sec(0), _min(0), _hrs(0) {}

TimeOfDay::TimeOfDay(uint sec, uint min, uint hrs)
    : _sec(sec), _min(min), _hrs(hrs) {
    // Round appropriately
    round();
}

TimeOfDay::TimeOfDay(double dr) {
    if (dr < 0) {
        // Raise exception
    }
    set(dr);
}

std::string TimeOfDay::two_digits(int val) {
    std::string output;

    if (val < 10)
        output += "0";
    output += std::to_string(val);
    return output;
}

void TimeOfDay::round() {
    _min += _sec / 60; // Make use of integer division here
    _sec = _sec % 60;

    _hrs += _min / 60;
    _min = _min % 60;

    _hrs = _hrs % 24;
}

// Explicit casts
TimeOfDay::operator std::string() {
    return two_digits(_hrs) + ":" + two_digits(_min) + ":" + two_digits(_sec);
}

TimeOfDay::operator int() { return _hrs * 3600 + _min * 60 + _sec; }
TimeOfDay::operator double() { return _hrs + 0.01 * _min; }

// Getters
uint TimeOfDay::sec() { return _sec; }
uint TimeOfDay::min() { return _min; }
uint TimeOfDay::hrs() { return _hrs; }

void TimeOfDay::set(double dr) {
    _sec = 0;
    _hrs = ((uint)std::floor(dr));
    _min = (uint)100 * (dr - _hrs) + 0.5;
    round();
}

// Overloaded operators
TimeOfDay& TimeOfDay::operator++() {
    *this += 1;
    return *this;
}

TimeOfDay TimeOfDay::operator++(int) {
    TimeOfDay old = *this;
    (*this)++;
    return old;
}

TimeOfDay& TimeOfDay::operator+=(const TimeOfDay& rhs) {
    _sec += rhs._sec;
    _min += rhs._min;
    _hrs += rhs._hrs;
    round();
    return *this;
}

TimeOfDay& TimeOfDay::operator+=(const int& rhs) {
    _sec += rhs;
    round();
    return *this;
}

bool operator==(const TimeOfDay& lhs, const TimeOfDay& rhs) {
    return ((lhs._hrs == rhs._hrs) && (lhs._min == rhs._min) &&
            (lhs._sec == rhs._sec));
}

TimeOfDay operator+(TimeOfDay lhs, const TimeOfDay& rhs) {
    return TimeOfDay(lhs._sec + rhs._sec, lhs._min + rhs._min,
                     lhs._hrs + rhs._hrs);
};
TimeOfDay operator+(TimeOfDay lhs, const int& rhs) {
    return TimeOfDay(lhs._sec + rhs, lhs._min, lhs._hrs);
};

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
