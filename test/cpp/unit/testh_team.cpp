#define BOOST_TEST_DYN_LINK

#include "testmatch/enums.hpp"
#include "testmatch/team.hpp"

#include <boost/test/parameterized_test.hpp>
#include <boost/test/unit_test.hpp>
#include <string>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_header_player)

BOOST_AUTO_TEST_CASE(testclass_player) {

    Player tp_bat(
        "Marnus", "Labuschagne", "M",
        {23, 63.43, 56.52, 756, 38.66, 63.0, 3.68, right, right, legbreak});

    BOOST_TEST(tp_bat.get_initials() == "M");
    BOOST_TEST(tp_bat.get_full_initials() == "M Labuschagne");
    BOOST_TEST(tp_bat.get_last_name() == "Labuschagne");
    BOOST_TEST(tp_bat.get_full_name() == "Marnus Labuschagne");

    BOOST_TEST(tp_bat.get_innings() == 23);
    BOOST_TEST(tp_bat.get_bat_avg() == 63.43);
    BOOST_TEST(tp_bat.get_bat_sr() == 56.52);

    BOOST_TEST(tp_bat.get_balls_bowled() == 756);
    BOOST_TEST(tp_bat.get_bowl_avg() == 38.66);
    BOOST_TEST(tp_bat.get_bowl_sr() == 63.0);
    BOOST_TEST(tp_bat.get_bowl_econ() == 3.68);

    BOOST_TEST(tp_bat.get_bat_arm() == right);
    BOOST_TEST(tp_bat.get_bowl_arm() == right);
    BOOST_TEST(tp_bat.get_bowl_type() == legbreak);
}

BOOST_AUTO_TEST_SUITE_END()
