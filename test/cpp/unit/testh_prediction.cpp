#define BOOST_TEST_DYN_LINK

#include <boost/test/parameterized_test.hpp>
#include <boost/test/unit_test.hpp>
#include <testmatch/cards.hpp>
#include <testmatch/enums.hpp>
#include <testmatch/models.hpp>

BOOST_AUTO_TEST_SUITE(test_header_prediction)

struct F {
    F() {
        bat = {50, 50, right, 0, 0, 0, 0};
        bowl = {25, 60, right, fast_med, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        match = {};
    };

    BatStats bat;
    BowlStats bowl;
    MatchStats match;
};

using namespace boost::unit_test;
using namespace prediction;

// Helper functions for checking probabilities
void check_single_prob(double prob) { BOOST_TEST((0 <= prob && prob <= 1)); }

template <typename S>
void check_probs(std::map<S, double> probs) {
    double sum = 0;
    for (auto const& [key, val] : probs) {
        check_single_prob(val);
        sum += val;
    }
    BOOST_TEST(sum == 1);
}

BOOST_FIXTURE_TEST_CASE(test_preds_sum, F) {
    // Ensure all prediction functions returning probabilities give values
    // between 0 and 1 and which sum to 1.

    // Toss choice
    check_single_prob(toss_elect(0.5));

    // Wicket
    check_single_prob(wkt(bat, bowl, match));

    // Extras
    check_probs<std::string>(extras(bat, bowl, match));

    // Runs scored
    check_probs<std::string>(bat_runs(bat, bowl, match));
    check_probs<std::string>(byes_runs(bat, bowl, match));
    check_probs<std::string>(legbyes_runs(bat, bowl, match));
    check_probs<std::string>(wides_runs(bat, bowl, match));
    check_probs<std::string>(noballs_runs(bat, bowl, match));

    // Full delivery wrapper
    check_probs<std::string>(delivery(bat, bowl, match));

    // Dismissal type
    check_probs<DismType>(wkt_type(bowl.bowl_type));

    // Declaration
    check_single_prob(declaration(250, 2000, true, 3));

    // Follow-on
    check_single_prob(follow_on(250));
}

BOOST_AUTO_TEST_SUITE_END()