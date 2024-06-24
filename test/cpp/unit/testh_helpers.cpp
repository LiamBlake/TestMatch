#define BOOST_TEST_DYN_LINK

#include <boost/test/parameterized_test.hpp>
#include <boost/test/unit_test.hpp>
//#include <boost/mpl/list.hpp>

#include "testmatch/enums.hpp"
#include "testmatch/helpers.hpp"

#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

BOOST_AUTO_TEST_SUITE(test_header_utility)

BOOST_AUTO_TEST_CASE(testfunc_split_str) {
    std::vector<std::string> expected;

    // Boundary case - empty string
    BOOST_TEST(split_str("").empty());

    // Boundary case - just delimeter
    expected.push_back("");
    expected.push_back("");
    BOOST_TEST(split_str(",") == expected);
    expected.clear();

    // Boundary case - no delimeter
    expected.push_back("test");
    BOOST_TEST(split_str("test") == expected);
    expected.clear();

    // Standard case - default delimeter
    expected.push_back("a");
    expected.push_back("b");
    expected.push_back("c");
    BOOST_TEST(split_str("a,b,c") == expected);

    // Standard case - delimeter at end
    expected.push_back("");
    BOOST_TEST(split_str("a,b,c,") == expected);

    // Standard case - delimeter at start
    expected.pop_back();
    expected.insert(expected.begin(), "");
    BOOST_TEST(split_str(",a,b,c") == expected);
    expected.clear();

    // Standard case - other delimeter
    expected.push_back("1");
    expected.push_back("2");
    expected.push_back("3");
    BOOST_TEST(split_str("1 2 3", " ") == expected);

    // Standard case - other delimeter
    BOOST_TEST(split_str("1thisisadelimeter2thisisadelimeter3",
                         "thisisadelimeter") == expected);
}

BOOST_AUTO_TEST_CASE(testfunc_balls_to_ov) {
    std::pair<int, int> out = balls_to_ov(0);
    BOOST_TEST((out.first == 0) & (out.second == 0));

    out = balls_to_ov(1);
    BOOST_TEST((out.first == 0) & (out.second == 1));

    out = balls_to_ov(6);
    BOOST_TEST((out.first == 1) & (out.second == 0));

    out = balls_to_ov(11);
    BOOST_TEST((out.first == 1) & (out.second == 5));

    out = balls_to_ov(7);
    BOOST_TEST((out.first == 1) & (out.second == 1));

    out = balls_to_ov(480);
    BOOST_TEST((out.first == 80) & (out.second == 0));
}

BOOST_AUTO_TEST_CASE(testfunc_is_slow_bowler) {
    BowlType types[6] = {med, med_fast, fast_med, fast, offbreak, legbreak};
    bool expected[6] = {true, false, false, false, true, true};

    for (int i = 0; i < 6; i++) {
        BOOST_TEST(is_slow_bowler(types[i]) == expected[i]);
    }
}

// Fixtures for map methods
struct F {
    F() { prob_map = {{"a", 0.1}, {"b", 0.5}, {"c", 0.4}, {"d", 0.2}}; };

    std::map<std::string, double> prob_map;
};

BOOST_FIXTURE_TEST_CASE(testfunc_multiply, F) {
    multiply<std::string, double>(prob_map, 2);
    BOOST_TEST(prob_map["a"] == 0.2);
    BOOST_TEST(prob_map["b"] == 1);
    BOOST_TEST(prob_map["c"] == 0.8);
    BOOST_TEST(prob_map["d"] == 0.4);
}

BOOST_FIXTURE_TEST_CASE(testfunc_normalise_to_ref, F) {
    normalise_to_ref<std::string, double>(prob_map, "d");
    BOOST_TEST(prob_map["a"] == 0.125);
    BOOST_TEST(prob_map["b"] == 0.625);
    BOOST_TEST(prob_map["c"] == 0.5);
    BOOST_TEST(prob_map["d"] == 0.2);
}

BOOST_AUTO_TEST_SUITE_END()