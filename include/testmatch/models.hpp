#ifndef PREDICTION_H
#define PREDICTION_H

#include "cards.hpp"
#include "enums.hpp"
#include "team.hpp"

#include <string>
#include <vector>

namespace Model {

extern int NUM_DELIV_OUTCOMES;
extern std::vector<std::string> DELIV_OUTCOMES;
extern int NUM_DISM_MODES;
extern std::vector<DismType> DISM_MODES_STATIC;

/**
 * @brief Determine the probability of electing to bat at the toss
 *
 * @param spin_factor The spin factor describing the pitch conditions
 * @return double Probability of electing to bat
 */
double MODEL_TOSS_ELECT(double spin_factor);

/**
 * @brief
 *
 * @param bat
 * @param bowl
 * @param match
 * @return
 */
double prob_wkt(BatStats bat, BowlStats bowl, MatchStats match);

/**
 * @brief
 *
 * @param bat
 * @param bowl
 * @param match
 * @return double*
 */
double* MODEL_DELIVERY(BatStats bat, BowlStats bowl);

/**
 * @brief
 *
 * @param bowltype
 * @return int
 */
DismType MODEL_WICKET_TYPE(BowlType bowltype);

/**
 * @brief Evaluates the "value" in bringing a bowler into the attack, based on
 * their ability and fatigue
 *
 * @param bowl_avg Bowling average of the bowler in question
 * @param bowl_sr Bowling strike rate of the bowler in question
 * @param fatigue Current fatigue of the bowler in question
 * @return double Measure of how much "value" bringing that bowler on brings
 */
double OBJ_AVG_FATIG(double bowl_avg, double bowl_sr, double fatigue);

/**
 * @brief Determine the probability of declaring on a given ball.
 *
 * @param lead
 * @param match_balls
 * @param is_wkt
 * @param innings
 * @return double
 */
double MODEL_DECLARATION(int lead, int match_balls, bool is_wkt, int innings);

/**
 * @brief Generate a probability for enforcing the follow-on, using the lead
 *
 * @param lead Lead of bowling team at end of previous innings.
 * @return Probability of enforcing the follow-on
 */
double MODEL_FOLLOW_ON(int lead);

}; // namespace Model

#endif // PREDICTION_H
