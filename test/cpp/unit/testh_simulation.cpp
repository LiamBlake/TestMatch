#define BOOST_TEST_DYN_LINK

#include <boost/test/parameterized_test.hpp>
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <exception>
#include <iostream>
#include <string>
#include <utility>

#define private public // Illegal command :(

#include "fixtures.hpp"
#include "testmatch/cards.hpp"
#include "testmatch/models.hpp"
#include "testmatch/simulation.hpp"
#include "testmatch/team.hpp"

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_header_simulation)

BOOST_FIXTURE_TEST_CASE(testclass_battingmanager, F_TeamAus) {
    // Create batting cards for each player
    BatterCard** cards = create_batting_cards(&aus);

    for (int i = 0; i < 11; i++) {
        delete cards[i];
    }
}

BOOST_FIXTURE_TEST_CASE(testclass_bowlingmanager, F_TeamNZ) {
    // Create bowler cards for each player
    BowlerCard** cards = create_bowling_cards(&nz);

    // Test object
    BowlingManager bm;
    bm.set_cards(cards);

    // Check that cards have been passed over correctly
    for (int i = 0; i < 11; i++) {
        BOOST_TEST(bm.cards[i] == cards[i]);
    }

    // Bowler getters
    BOOST_TEST(bm.new_pacer(nullptr, nullptr) == cards[7]);
    BOOST_TEST(bm.new_pacer(cards[7], cards[8]) == cards[9]);
    BOOST_TEST(bm.new_spinner(nullptr, nullptr) == cards[6]);
    BOOST_TEST(bm.part_timer(nullptr, nullptr) == cards[2]);

    for (int i = 0; i < 11; i++) {
        delete cards[i];
    }
}

BOOST_FIXTURE_TEST_CASE(testclass_innings, F_Pregame) {
    // Create an innings
    Innings inns(pregame.home_team, pregame.away_team, 0, &pf);

    // Check initialisation of innings
    BOOST_TEST(inns.striker->get_player_ptr() == &a1 |
               inns.striker->get_player_ptr() == &a2);
    BOOST_TEST(inns.nonstriker->get_player_ptr() == &a1 |
               inns.nonstriker->get_player_ptr() == &a2);
    BOOST_TEST(inns.striker != inns.nonstriker);

    BOOST_TEST(inns.bowl1->get_player_ptr() == &b11);
    BOOST_TEST(inns.bowl2->get_player_ptr() == &b9);

    // Simulate a delivery
}

BOOST_AUTO_TEST_CASE(testfeature_followon) {

    // Cases where follow-on is not an option
    BOOST_CHECK(!Match::DECIDE_FOLLOW_ON(0));
    BOOST_CHECK(!Match::DECIDE_FOLLOW_ON(-201));
    BOOST_CHECK(!Match::DECIDE_FOLLOW_ON(199));

    // Ensure fit matches that expected by R
    double eps = 0.0001;
    BOOST_TEST(abs(Model::MODEL_FOLLOW_ON(200) - 0.1386838) < eps);
    BOOST_TEST(abs(Model::MODEL_FOLLOW_ON(250) - 0.3812311) < eps);
    BOOST_TEST(abs(Model::MODEL_FOLLOW_ON(350) - 0.7442012) < eps);
    BOOST_TEST(abs(Model::MODEL_FOLLOW_ON(500) - 0.9046413) < eps);
}

BOOST_FIXTURE_TEST_CASE(testclass_simulation, F_Pregame) {
    // NOTE: These tests mainly check higher-level aspects of the simulation, to
    // ensure that things are running as expected. Lower-level functionality
    // should be captured by tests of the individual objects.

    // Simulate an innings several times, to ensure running without error
    for (int i = 0; i < 50; i++) {
        Innings inn(&aus, &nz, 0, &pf);
        inn.simulate(true);

        // Check that innings has closed appropriately
        BOOST_TEST(inn.wkts == 10);
        // TODO: Check for declaration flag when implemented

        // Check that things make sense, e.g.
        // Batting team have not somehow lost runs.
        BOOST_TEST(inn.lead >= 0);

        // The overs bowled by individual bowlers adds up to the innings total
        int sum_balls = 0;
        for (int i = 0; i < 11; i++) {
            sum_balls += inn.bowlers[i]->get_sim_stats().legal_balls;
        }
        BOOST_TEST(inn.legal_delivs == sum_balls);

        // The individual batter scores and extras add up to team total.
        int sum_runs = inn.extras.total();
        for (int i = 0; i < 11; i++) {
            sum_runs += inn.batters[i]->get_sim_stats().runs;
        }
        BOOST_TEST(inn.team_score == sum_runs);
    }

    // Run the full simulation several times, to ensure that the simulation runs
    // without error.
    for (int i = 0; i < 50; i++) {
        Match sim(pregame);
        sim.start();
    }
}

BOOST_AUTO_TEST_SUITE_END()
