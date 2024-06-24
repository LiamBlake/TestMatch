// -*- lsst-c++ -*-
/* demonstrative_test.cpp
 *
 * This is where the fun testing is! This is where I use the simulation to
 * simulate a test match between Australia and New Zealand at Lord's (think WTC
 * final, one can hope), printing ball-by-ball and results. This will be
 * progressively developed as I build the simulation.
 *
 * To compile manually:
 * gcc -o ../bin/demonstrative.exe ../src/Cards.cpp ../src/Player.cpp
 * ../src/Simulation.cpp demonstrative_test.cpp -I ../include
 */

#include <ctime>
#include <iostream>
#include <string>
#include <testmatch/cards.hpp>
#include <testmatch/helpers.hpp>
#include <testmatch/simulation.hpp>
#include <testmatch/team.hpp>

int main() {

    // Reseed RNG
    srand(time(NULL));

    // World XI players
    Player a1("Alastair", "Cook", "AN",
              {291, 45.35, 46.95, 18, 7, 18.0, 2.33, right, right, med});
    Player a2("Will", "Pucovski", "WJ",
              {1, 42.54, 60.21, 0, 1000, 1000, 4.00, right, right, med});
    Player a3(
        "Marnus", "Labuschagne", "M",
        {23, 63.43, 56.52, 756, 38.66, 63.0, 3.68, right, right, legbreak});
    Player a4("Virat", "Kohli", "V",
              {153, 52.37, 52.37, 175, 1000, 1000, 2.88, right, right, med});
    Player a5(
        "Glenn", "Maxwell", "GJ",
        {14, 46.07, 59.47, 462, 42.62, 57.70, 4.42, right, right, offbreak});
    Player a6("Mohammad", "Rizwan", "M",
              {22, 43.15, 51.57, 0, 1000, 1000, 4.00, right, right, med});
    Player a7(
        "Colin", "de Grandhomme", "C",
        {36, 37.03, 81.72, 3683, 31.63, 78.30, 2.42, right, right, med_fast});
    Player a8(
        "Pat", "Cummins", "PJ",
        {44, 17.02, 38.51, 6761, 21.82, 47.2, 2.76, right, right, fast_med});
    Player a9("Anrich", "Nortje", "AA",
              {18, 7.42, 24.07, 1894, 29.84, 48.50, 3.68, right, right, fast});
    Player a10(
        "Lasith", "Embuldeniya", "L",
        {16, 8.06, 38.53, 3231, 37.20, 65.90, 3.38, left, left, legbreak});
    Player a11(
        "Shaheen Shah", "Afridi", "S",
        {24, 4.68, 31.67, 3300, 29.58, 56.80, 3.12, left, left, fast_med});
    Team world = {"World XI", &a1, &a2,  &a3,  &a4, &a5, &a6, &a7,
                  &a8,        &a9, &a10, &a11, 3,   5,   7,   10};

    Player b1("Tom", "Latham", "TWM",
              {92, 42.34, 46.66, 0, 1000, 1000, 4.00, left, left, med});
    Player b2("Tom", "Blundell", "TA",
              {11, 47.22, 49.47, 18, 1000, 1000, 4.33, right, right, offbreak});
    Player b3(
        "Kane", "Williamson", "KS",
        {140, 50.99, 51.63, 2103, 40.62, 72.5, 3.36, right, right, offbreak});
    Player b4("Ross", "Taylor", "LRPL",
              {178, 46.1, 60, 96, 24, 48, 3, right, right, offbreak});
    Player b5("Henry", "Nicholls", "HM",
              {50, 39.7, 49.39, 0, 1000, 1000, 4.00, left, right, offbreak});
    Player b6("BJ", "Watling", "BJ",
              {110, 38.5, 42.35, 0, 1000, 1000, 4.00, right, right, med});
    Player b7("Mitchell", "Santner", "MJ",
              {29, 25.55, 42.36, 3746, 44.71, 96, 2.79, left, left, legbreak});
    Player b8("Kyle", "Jamieson", "KA",
              {6, 21.47, 55, 1202, 21.14, 42.2, 3, right, right, fast_med});
    Player b9(
        "Tim", "Southee", "TG",
        {106, 17.37, 85.84, 16393, 29, 57.7, 3.01, right, right, med_fast});
    Player b10(
        "Neil", "Wagner", "N",
        {63, 12.5, 44.88, 10743, 26.6, 52.1, 3.06, left, left, med_fast});
    Player b11(
        "Trent", "Boult", "TA",
        {82, 15.2, 56.86, 14874, 27.65, 55.7, 2.97, right, left, fast_med});
    Team alltime = {"New Zealand", &b1, &b2,  &b3,  &b4, &b5, &b6, &b7,
                    &b8,           &b9, &b10, &b11, 2,   5,   10,  8};

    // Venue
    PitchFactors lords_pf = {0.75995148, 0.24004852};
    Venue lords = {"Lords", "London", "ENG", &lords_pf};
    Pregame pregame = {&lords, &world, &alltime};

    // Create a Match object
    Match m(pregame);

    // Simulation
    m.pregame();
    m.start(false);

    // std::cout.precision(3);
    m.print_all();

    return 0;
}
