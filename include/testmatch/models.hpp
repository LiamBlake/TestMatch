#ifndef PREDICTION_H
#define PREDICTION_H

#include "cards.hpp"
#include "enums.hpp"
#include "team.hpp"

#include <map>
#include <string>
#include <vector>

namespace prediction {

/**
 * @brief Determine the probability of electing to bat at the toss
 *
 * @param spin_factor The spin factor describing the pitch conditions
 * @return double Probability of electing to bat
 */
double toss_elect(double spin_factor);

/**
 * @brief
 *
 * @param bat
 * @param bowl
 * @param match
 * @return
 */
double wkt(BatStats bat, BowlStats bowl, MatchStats match);

/**
 * @brief
 *
 * @param bat
 * @param bowl
 * @param match
 * @return std::map<std::string, double>
 */
std::map<std::string, double> extras(BatStats bat, BowlStats bowl,
                                     MatchStats match);

/**
 * @brief
 *
 * @param bat
 * @param bowl
 * @param match
 * @return std::map<std::string, double>
 */
std::map<std::string, double> bat_runs(BatStats bat, BowlStats bowl,
                                       MatchStats match);

/**
 * @brief
 *
 * @param bat
 * @param bowl
 * @param match
 * @return std::map<std::string, double>
 */
std::map<std::string, double> byes_runs(BatStats bat, BowlStats bowl,
                                        MatchStats match);

/**
 * @brief
 *
 * @param bat
 * @param bowl
 * @param match
 * @return std::map<std::string, double>
 */
std::map<std::string, double> legbyes_runs(BatStats bat, BowlStats bowl,
                                           MatchStats match);

/**
 * @brief
 *
 * @param bat
 * @param bowl
 * @param match
 * @return std::map<std::string, double>
 */
std::map<std::string, double> wides_runs(BatStats bat, BowlStats bowl,
                                         MatchStats match);

/**
 * @brief
 *
 * @param bat
 * @param bowl
 * @param match
 * @return std::map<std::string, double>
 */
std::map<std::string, double> noballs_runs(BatStats bat, BowlStats bowl,
                                           MatchStats match);

/**
 * @brief Perform all required probability calculations for a delivery,
 * returning probabilites of every possible outcome.
 *
 * @param bat Statistics and state of the batter facing the delivery.
 * @param bowl Statistics and state of the bowler bowling the delivery.
 * @param match Statistics and state of the match prior to the delivery.
 * @return std::map<std::string, double> Mapping between outcome strings and
 * corresponding probabilities.
 */
std::map<std::string, double> delivery(BatStats bat, BowlStats bowl,
                                       MatchStats match);

/**
 * @brief Generate a probability for the type of dismissal when a wicket falls,
 * based on the bowling type.
 *
 * @param bowltype The bowling type of the bowler who took the wicket.
 * @return std::map<DismType, double> A mapping between each dismissal type and
 * the corresponding probabilities.
 */
std::map<DismType, double> wkt_type(BowlType bowltype);

/**
 * @brief Determine the probability of declaring on a given ball.
 *
 * @param lead
 * @param match_balls
 * @param is_wkt
 * @param innings
 * @return double
 */
double declaration(int lead, int match_balls, bool is_wkt, int innings);

/**
 * @brief Generate a probability for enforcing the follow-on, using the lead
 *
 * @param lead Lead of bowling team at end of previous innings.
 * @return Probability of enforcing the follow-on
 */
double follow_on(int lead);

// TODO: This entire approach will be reconsidered. Change the naming then.
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

}; // namespace prediction

#endif // PREDICTION_H
