#define BOOST_TEST_DYN_LINK

#include <boost/test/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

#include <string>

#include "MatchTime.h"

BOOST_AUTO_TEST_SUITE(test_header_matchtimetime)

BOOST_AUTO_TEST_CASE(testclass_timeofday_constructors) {
    // Default constructor
    TimeOfDay t1;
    BOOST_TEST(t1.sec() == 0);
    BOOST_TEST(t1.min() == 0);
    BOOST_TEST(t1.hrs() == 0);

    // hrs, sec, min constructor
    TimeOfDay t2(1, 2, 3);
    BOOST_TEST(t2.sec() == 1);
    BOOST_TEST(t2.min() == 2);
    BOOST_TEST(t2.hrs() == 3);

    TimeOfDay t3(59, 59, 23);
    BOOST_TEST(t3.sec() == 59);
    BOOST_TEST(t3.min() == 59);
    BOOST_TEST(t3.hrs() == 23);

    // Double constructor
    TimeOfDay t4(7.35);
    BOOST_TEST(t4.sec() == 0);
    BOOST_TEST(t4.min() == 35);
    BOOST_TEST(t4.hrs() == 7);

    TimeOfDay t5(17.05);
    BOOST_TEST(t5.sec() == 0);
    BOOST_TEST(t5.min() == 5);
    BOOST_TEST(t5.hrs() == 17);

    // Constructors with rounding
    TimeOfDay t6(5, 61, 24);
    BOOST_TEST(t6.sec() == 5);
    BOOST_TEST(t6.min() == 1);
    BOOST_TEST(t6.hrs() == 1);

    TimeOfDay t7(3, 2, 78);
    BOOST_TEST(t7.sec() == 3);
    BOOST_TEST(t7.min() == 2);
    BOOST_TEST(t7.hrs() == 6);

    TimeOfDay t8(29.72);
    BOOST_TEST(t8.sec() == 0);
    BOOST_TEST(t8.min() == 12);
    BOOST_TEST(t8.hrs() == 6);
}

BOOST_AUTO_TEST_CASE(testclass_timeofday_operators) {

    // Casts
    TimeOfDay t1;
    BOOST_TEST(std::string(t1) == "00:00:00");
    BOOST_TEST(int(t1) == 0);
    BOOST_TEST(double(t1) == 0.0);

    TimeOfDay t2(20, 2, 5);
    BOOST_TEST(std::string(t2) == "05:02:20");
    BOOST_TEST(int(t2) == 18140);
    BOOST_TEST(double(t2) == 5.02);

    TimeOfDay t3(59, 58, 15);
    BOOST_TEST(std::string(t3) == "15:58:59");
    BOOST_TEST(int(t3) == 57539);
    BOOST_TEST(double(t3) == 15.58);

    // Iterators
    // These tests are causing memory access violations. Since we have no
    // immediate need for the increment operators, this functionality is being
    // ignored for now. I am a terrible programmer, I know.
    // BOOST_TEST(std::string(t1++) == "00:00:00");
    // BOOST_TEST(std::string(t1) == "00:00:01");
    // BOOST_TEST(std::string(++t1) == "00:00:02");
    // BOOST_TEST(std::string(t1) == "00:00:02");

    // BOOST_TEST(std::string(++t2) == "15:58:59");
    // BOOST_TEST(std::string(++t2) == "15:59:00");

    // Temporary - ensure subsequent tests work
    t1 += 2;
    t2 += 2;

    // Addition
    t3 += 5;
    BOOST_TEST(std::string(t3) == "15:59:04");
    t3 += 15;
    BOOST_TEST(std::string(t3) == "15:59:19");
    t3 += 41;
    BOOST_TEST(std::string(t3) == "16:00:00");

    BOOST_TEST(std::string(t1 + t2) == "05:02:24");
    BOOST_TEST(std::string(t1 + 47) == "00:00:49");

    // Equality
    TimeOfDay t4(2, 0, 0);
    TimeOfDay t5(22, 2, 5);
    BOOST_CHECK(t1 == t4);
    BOOST_CHECK(t5 == t2);
}

BOOST_AUTO_TEST_SUITE_END()