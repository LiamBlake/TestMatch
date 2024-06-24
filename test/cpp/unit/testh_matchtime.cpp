#define BOOST_TEST_DYN_LINK

#include "testmatch/matchtime.hpp"

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <string>

BOOST_AUTO_TEST_SUITE(test_class_timeofday)

struct F {
    F() : t1(1, 2, 3), t2((float)10.30){};

    TimeOfDay t1;
    TimeOfDay t2;
};

BOOST_FIXTURE_TEST_CASE(constructor_values, F) {
    BOOST_TEST(t1.get_hour() == 1);
    BOOST_TEST(t1.get_min() == 2);
    BOOST_TEST(t1.get_sec() == 3);
}

BOOST_FIXTURE_TEST_CASE(constructor_float, F) {
    // GIVEN: A float representation of a time.
    // WHEN: Constructing a TimeOfDay object from that float.
    // THEN: The object is constructed correctly.
    BOOST_TEST(t2.get_hour() == 10);
    BOOST_TEST(t2.get_min() == 30);
    BOOST_TEST(t2.get_sec() == 0);
}

BOOST_FIXTURE_TEST_CASE(string, F) {
    BOOST_TEST(std::string(t1) == "1:02 am");
    BOOST_TEST(std::string(t2) == "10:30 am");

    t1 += 12 * 3600;
    BOOST_TEST(std::string(t1) == "1:02 pm");

    TimeOfDay t3(0, 0, 0);
    BOOST_TEST(std::string(t3) == "12:00 am");

    t3 += 12 * 3600;
    BOOST_TEST(std::string(t3) == "12:00 pm");
}

BOOST_FIXTURE_TEST_CASE(set, F) {
    // GIVEN: A TimeOfDay object and a new time to set.
    // WHEN: Setting the object to the new time.
    // THEN: The time is updated correctly.
    t1.set(5, 4, 3);
    BOOST_TEST(t1.get_hour() == 5);
    BOOST_TEST(t1.get_min() == 4);
    BOOST_TEST(t1.get_sec() == 3);

    t1.set(15.45);
    BOOST_TEST(t1.get_hour() == 15);
    BOOST_TEST(t1.get_min() == 45);
    BOOST_TEST(t1.get_sec() == 0);
}

BOOST_FIXTURE_TEST_CASE(operator_peq, F) {
    // GIVEN: A TimeOfDay object.
    // WHEN: Using the += operator.
    // THEN: The time is incremented correctly.
    t1 += 1;
    BOOST_TEST(t1.get_hour() == 1);
    BOOST_TEST(t1.get_min() == 2);
    BOOST_TEST(t1.get_sec() == 4);

    t1 += 60;
    BOOST_TEST(t1.get_hour() == 1);
    BOOST_TEST(t1.get_min() == 3);
    BOOST_TEST(t1.get_sec() == 4);

    t1 += 3600;
    BOOST_TEST(t1.get_hour() == 2);
    BOOST_TEST(t1.get_min() == 3);
    BOOST_TEST(t1.get_sec() == 4);

    t1 += 61;
    BOOST_TEST(t1.get_hour() == 2);
    BOOST_TEST(t1.get_min() == 4);
    BOOST_TEST(t1.get_sec() == 5);

    t1 += 3661;
    BOOST_TEST(t1.get_hour() == 3);
    BOOST_TEST(t1.get_min() == 5);
    BOOST_TEST(t1.get_sec() == 6);
}

BOOST_FIXTURE_TEST_CASE(operator_m, F) {
    // GIVEN: Two TimeOfDay objects or a float to compare.
    // WHEN: Getting the difference with the - operator.
    // THEN: The difference is seconds is given.
    int diff1 = t1 - t1;
    int diff2 = 10.30 - t2;
    int diff3 = t2 - t1;
    int diff4 = t1 - t2;
    int diff5 = t2 - 4.45;

    BOOST_TEST(diff1 == 0);
    BOOST_TEST(diff2 == 0);
    BOOST_TEST(diff3 == 34077);
    BOOST_TEST(diff4 == -34077);
    BOOST_TEST(diff5 == 20700);
}

BOOST_FIXTURE_TEST_CASE(operator_eq, F) {
    bool eq1 = (t1 == t1);
    bool eq2 = (t1 == t2);
    bool eq3 = (t1 == 1.02);
    bool eq4 = (t2 == 10.30);

    BOOST_TEST(eq1);
    BOOST_TEST(!eq2);
    BOOST_TEST(!eq3);
    BOOST_TEST(eq4);
}

BOOST_FIXTURE_TEST_CASE(operator_leq, F) {
    bool l1 = (t1 > t1);
    bool l2 = (t1 > t2);
    bool l3 = (t2 > t1);
    bool l4 = (t1 > 1.00);
    bool l5 = (t2 > 10.30);
    bool l6 = (t1 >= t1);
    bool l7 = (t1 >= t2);
    bool l8 = (t2 >= 10.30);
    bool l9 = (t2 >= 8.15);
    bool l10 = (t2 >= 15.05);

    BOOST_TEST(!l1);
    BOOST_TEST(!l2);
    BOOST_TEST(l3);
    BOOST_TEST(l4);
    BOOST_TEST(!l5);
    BOOST_TEST(l6);
    BOOST_TEST(!l7);
    BOOST_TEST(l8);
    BOOST_TEST(l9);
    BOOST_TEST(!l10);
}

BOOST_AUTO_TEST_SUITE_END()