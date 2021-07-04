// -*- lsst-c++ -*-
/* Utility.h
 *
 * Contains various helper functions used throughout the program. Includes
 * functions for converting between strings and their encodings as integers, and
 * vice-versa. These are used primarily in conversions from simulation backend
 * to printed/displayed results (e.g. a scorecard or ball-by-ball description).
 *
 */

#ifndef UTILITY_H
#define UTILITY_H

#include "enums.hpp"

#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// For the sake of formatting, allow integers and strings to be added
// std::string operator+() {};

/* Add new entry to end of array - reallocates memory dynamically.
   Deletes the old array and returns a pointer to the new.
   Arguments:
        T* arr - pointer to array of type T
        int old_length - length of array arr
        T new_entry - item to add to end of array arr

        This exists because overs can contain an arbitrary amount of balls
        (due to extras), but I don't want to use vectors since I will be wasting
        a lot of memory.
*/
template <class T>
inline T* arr_add_item(T* arr, int old_length, T new_entry) {

    // Reallocate array
    T* new_arr = new T[old_length + 1];
    if (old_length > 0) {

        for (int i = 0; i < old_length; i++) {
            new_arr[i] = arr[i];
        }
        delete[] arr;
    }

    delete[] arr;
    arr[old_length] = new_entry;

    return arr;
}

inline std::vector<std::string> split_str(std::string str,
                                          std::string delim = ",") {
    /* TODO: implement with string* return type to save memory,
                     ideally using the arr_add_item template defined in
                     Utility.h. Attempts at this have been giving strange
                     results, where the while loop won't iterate unless
                     cout << len << endl is included at the start of the
                     loop.
    */

    std::vector<std::string> token;

    // Check for boundary cases
    if (str.length() == 0) {
        return token;
    }

    size_t curr, prev = 0;
    curr = str.find(delim);

    // Check delim is actually present in string
    if (curr == std::string::npos) {
        token.push_back(str);
        return token;
    }

    while (curr != std::string::npos) {
        token.push_back(str.substr(prev, curr - prev));
        prev = curr + delim.length();
        curr = str.find(delim, prev);
    }

    // Add remaining string
    token.push_back(str.substr(prev, curr - prev));

    return token;
}

inline std::string join_str(std::vector<std::string> str_comp,
                            std::string join) {
    std::string output = "";
    for (std::string x : str_comp) {
        output += x + join;
    }

    // Remove last join added in above loop
    return output.substr(0, output.size() - join.size());
}

template <typename T>
T sample_cdf(T* values, int length, double* dist) {

    // Generate random number
    double r = ((double)rand() / (RAND_MAX));

    // Iterate through distribution until first entry > r
    int i = 0;
    while (++i < length && r > dist[i])
        ;

    return values[i - 1];
}

template <typename S>
S sample_pf_map(std::map<S, double> probs) {
    // Generate random number
    double r = ((double)rand() / (RAND_MAX));

    // Iterate through CDF until the first entry > r is found
    double sum_prob = 0;
    for (auto const& [key, val] : probs) {
        if (r > sum_prob)
            return key;
        sum_prob += val;
    }
};

// Converts ball count to overs and balls
inline std::pair<int, int> balls_to_ov(unsigned int balls) {
    std::pair<int, int> output((int)balls / 6, balls % 6);

    return output;
}

// Generates a realisation of a truncated exponential distribution
inline double rtexp(double mean, double min, double max) {
    double Fmin = exp(min);
    double Fmax = exp(max);

    // Generate uniform random number
    double r = ((double)rand() / (RAND_MAX));

    // Inverse sampling
    double p = r * (Fmax - Fmin) + Fmin;
    return -mean * log(1 - p);
}

template <typename T>
inline void delete_linkedlist(T* first) {
    T* next = first;
    while (next != nullptr) {
        T* tmp = next;
        next = next->get_next();
        delete tmp;
    }
}

/**
 * @brief Converts integer to ordinal string
 * @param num Integer to be converted
 * @return Converted string
 *
 * e.g. ordinal(1) returns "1st", ordinal(15) returns "15th"
 */
inline std::string ordinal(int num) {
    std::string output = std::to_string(num);

    char fin = output.back();
    if (fin == '1') {
        output += "st";
    } else if (fin == '2') {
        output += "nd";
    } else if (fin == '3') {
        output += "rd";
    } else {
        output += "th";
    }

    return output;
}

/**
 * @brief Determine whether a given bowling type is "slow" (i.e. spin or medium
 * pace)
 * @param bowl_type The bowling type, encoding as the appropriate enum.
 * @return A boolean indicating whether the bowling type is slow (true) or not
 * (false).
 *
 * The passed bowling type is checked via a switch statement.
 */
bool is_slow_bowler(BowlType bowl_type);

/**
 * @brief Performs Box-Cox transform given power parameter
 *
 * @param x value to transform
 * @param lambda power parameter
 * @return transformed value
 */
inline double boxcox(double x, double lambda) {
    if (x < 0) {
        // raise exception
        throw std::invalid_argument(
            "Value to transform must be greater than 0");
    }

    if (lambda == 0) {
        return log(x);
    } else {
        return (pow(x, lambda) - 1) / lambda;
    }
}

template <typename T>
void print_spaced(std::ostream& os, T t, const int& width) {
    os << std::left << std::setw(width) << std::setfill(' ') << t;
    // return os;
}

std::string print_rounded(double value, int precision = 2);

/**
 * @brief Normalise a map of probabilities
 *
 * @tparam T
 * @tparam S
 * @param map
 * @param ref_key
 */
template <typename S, typename T>
void normalise_to_ref(std::map<S, T>& map, S ref_key) {
    T ref_val = map[ref_key];
    for (auto const& [key, val] : map) {
        if (key != ref_key)
            map[key] = val / (1 - ref_val);
    }
};

template <typename S, typename T>
void multiply(std::map<S, T>& map, T mult) {
    for (auto const& [key, val] : map) {
        map[key] = mult * val;
    }
};

#endif // UTILITY_H
