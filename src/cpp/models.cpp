#include "testmatch/models.hpp"

#include "testmatch/helpers.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace prediction {

// Constants used internally within the prediction namespace
// TODO: Find a better way of avoiding hardcoding these within the prediction
// functions. Perhaps enums? This approach feels clunky.
// All possible types of extras
static const int N_EXTRAS = 5;
static const std::string EXTRAS[5] = {"legal", "b", "lb", "nb", "wd"};

// All possible outcomes when byes occurs
static const int N_OUTCOMES_BYE = 4;
static const std::string OUTCOMES_BYE[4] = {"1b", "2b", "3b", "4b"};

// All possible outcomes when legbyes occur
static const int N_OUTCOMES_LEGBYE = 4;
static const std::string OUTCOMES_LEGBYE[4] = {"1lb", "2lb", "3lb", "4lb"};

// All possible outcomes when a no-ball occurs.
static const int N_OUTCOMES_NOBALL = 7;
static const std::string OUTCOMES_NOBALL[7] = {"1nb", "2nb", "3nb", "4nb",
                                               "5nb", "6nb", "7nb"};

// All possible outcomes when wides occur
static const int N_OUTCOMES_WIDE = 5;
static const std::string OUTCOMES_WIDE[5] = {"1wd", "2wd", "3wd", "4wd", "5wd"};

// All possible outcomes on a legal delivery where no wicket falls
static const int N_OUTCOMES_LEGAL = 7;
static const std::string OUTCOMES_LEGAL[7] = {"0", "1", "2", "3",
                                              "4", "5", "6"};

static const int N_DISM_MODES = 6;
static const DismType DISM_MODES[6] = {bowled, caught,  c_and_b,
                                       lbw,    run_out, stumped};

/* Somewhat terrible fit to the toss elect probabilities in actual data
 * Note that spin_factor = 1 - seam_factor, so we only need to consider
 * the spin factor when calculating the probability. This is based on the
 * intuition that in extreme spinning conditions, a team will bat first,
 * whereas in extreme seaming conditions, a team will bowl first.
 *
 * Using a probability rather than a hard choice adds in that extra
 *uncertainty that appears whenever human decision is involved.
 *
 * This model, along with the entire pitch condition set-up, will probably
 * be eventually improved.
 **/
double toss_elect(double spin_factor) {
    // Exponential model
    double a = 0.05;
    return a * exp(log(0.9 / a) * spin_factor);
}

double wkt(BatStats bat, BowlStats bowl, MatchStats match) {
    double bat_sr = bat.career_strike_rate;
    double bat_avg = bat.career_bat_avg;
    if (bat_sr == 0)
        bat_sr = 50;
    if (bat_avg == 0)
        bat_avg = 5;

    return 0.5 * (bat_sr / (100 * bat_avg) + 1 / bowl.strike_rate);
}

std::map<std::string, double> extras(BatStats bat, BowlStats bowl,
                                     MatchStats match) {
    Arm bowl_arm = bowl.bowl_arm;
    BowlType bowl_type = bowl.bowl_type;
}

std::map<std::string, double> bat_runs(BatStats bat, BowlStats bowl,
                                       MatchStats match) {
    double bat_sr = bat.career_strike_rate;
}

std::map<std::string, double> byes_runs(BatStats bat, BowlStats bowl,
                                        MatchStats match) {}

std::map<std::string, double> legbyes_runs(BatStats bat, BowlStats bowl,
                                           MatchStats match) {}

std::map<std::string, double> wides_runs(BatStats bat, BowlStats bowl,
                                         MatchStats match) {}

std::map<std::string, double> noballs_runs(BatStats bat, BowlStats bowl,
                                           MatchStats match) {}

/**
 * @brief
 *
 * THIS WILL BE REPLACED WITH A MODEL WHICH ACTUALLY USES REAL DATA, RATHER THAN
 * MY OWN BLIND INTUITION
 *
 *
 */
double OBJ_AVG_FATIG(double bowl_avg, double bowl_sr, double fatigue) {
    return 3.0 / (1.0 / bowl_avg + 1.0 / bowl_sr + 1.0 / (fatigue + 1));
}

/**
 *
 */
double MODEL_DECLARATION(int lead, int match_balls, bool is_wkt, int innings) {
    return 0;
}

/**
 * Current model is a logistic regression on the lead. The lead is first
 * transformed with a Box-Cox transformation, then used to calculate the
 * probability. The model was fitted on historical data using R.
 *
 * The model is
 * \f[
 *  p = \frac{1}{1 + \exp{-1101.903 + 1058.466 \times\text{t_lead}}}.
 * \f]
 */
double follow_on(int lead) {
    // Logistic regression, fitted in R

    // Preprocessing of lead value
    double t_lead = boxcox(lead, -0.9561039); // Box-Cox transform

    // Fitted model
    double logit = -1101.903 + 1058.466 * t_lead;
    return 1 / (1 + exp(logit));
}

void merge_cond_prob(std::map<std::string, double>& probs_map,
                     std::map<std::string, double> cond_map, double prob) {
    multiply<std::string, double>(cond_map, prob);
    probs_map.merge(cond_map);
}

// Generates probability distribution for each possible outcome
std::map<std::string, double> delivery(BatStats bat, BowlStats bowl,
                                       MatchStats match) {
    std::map<std::string, double> probs;

    // Wicket probability
    probs["W"] = wkt(bat, bowl, match);

    // Probabilities of extras
    std::map<std::string, double> probs_extras = extras(bat, bowl, match);

    // Each outcome
    merge_cond_prob(probs, bat_runs(bat, bowl, match), probs_extras["legal"]);
    merge_cond_prob(probs, byes_runs(bat, bowl, match), probs_extras["b"]);
    merge_cond_prob(probs, legbyes_runs(bat, bowl, match), probs_extras["lb"]);
    merge_cond_prob(probs, noballs_runs(bat, bowl, match), probs_extras["nb"]);
    merge_cond_prob(probs, wides_runs(bat, bowl, match), probs_extras["wd"]);

    // Scale each probability w.r.t wicket probability
    normalise_to_ref<std::string, double>(probs, "W");
    return probs;
}

std::map<DismType, double> wkt_type(BowlType bowltype) {
    double DISM_MODE_SEAMER[6] = {0, 0.175, 0.815, 0.8291, 0.9731, 1};

    std::map<DismType, double> probs;
    if (is_slow_bowler(bowltype)) {
        probs[bowled] = 0.157;
        probs[caught] = 0.535;
        probs[c_and_b] = 0.0354;
        probs[lbw] = 0.2012;
        probs[run_out] = 0.0327;
        probs[stumped] = 0.0387;
    } else {
        probs[bowled] = 0.1755;
        probs[caught] = 0.64;
        probs[c_and_b] = 0.0141;
        probs[lbw] = 0.144;
        probs[run_out] = 0.0269;
        probs[stumped] = 0;
    }

    return probs;
}
} // namespace prediction
