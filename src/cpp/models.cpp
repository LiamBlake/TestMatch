/**
 * @file Model.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-03-23
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "testmatch/models.hpp"

#include "testmatch/helpers.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
namespace Model {

int NUM_DELIV_OUTCOMES = 22;
std::vector<std::string> DELIV_OUTCOMES = {
    "0", "1",  "1b",  "1lb", "1nb", "1wd", "2", "2b",  "2lb", "2nb", "2wd",
    "3", "3b", "3lb", "4",   "4b",  "4lb", "5", "5nb", "5wd", "6",   "W"};
int NUM_DISM_MODES = 6;
std::vector<DismType> DISM_MODES_STATIC = {bowled, caught,  c_and_b,
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
double MODEL_TOSS_ELECT(double spin_factor) {
    // Exponential model
    double a = 0.05;
    return a * exp(log(0.9 / a) * spin_factor);
}

double prob_wkt(BatStats bat, BowlStats bowl, MatchStats match) {
    double bat_sr = bat.career_strike_rate;
    double bat_avg = bat.career_bat_avg;
    if (bat_sr == 0)
        bat_sr = 50;
    if (bat_avg == 0)
        bat_avg = 5;

    return 0.5 * (bat_sr / (100 * bat_avg) + 1 / bowl.strike_rate);
}

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
double MODEL_FOLLOW_ON(int lead) {
    // Logistic regression, fitted in R

    // Preprocessing of lead value
    double t_lead = boxcox(lead, -0.9561039); // Box-Cox transform

    // Fitted model
    double logit = -1101.903 + 1058.466 * t_lead;
    return 1 / (1 + exp(logit));
}

// Generates probability distribution for each possible outcome
double* MODEL_DELIVERY(BatStats bat, BowlStats bowl) {
    double* output = new double[NUM_DELIV_OUTCOMES];

    // PLACEHOLDER - data proportions, hard-coded model
    if (is_slow_bowler(bowl.bowl_type)) {
        output[0] = 0;
        output[1] = 0.700414129;
        output[2] = 0.878619915;
        output[3] = 0.879463788;
        output[4] = 0.881999123;
        output[5] = 0.884047465;
        output[6] = 0.884303973;
        output[7] = 0.919880445;
        output[8] = 0.920519855;
        output[9] = 0.921326553;
        output[10] = 0.92133027;
        output[11] = 0.921341423;
        output[12] = 0.92863144;
        output[13] = 0.928873077;
        output[14] = 0.929029212;
        output[15] = 0.976230307;
        output[16] = 0.977791656;
        output[17] = 0.978115079;
        output[18] = 0.978234039;
        output[19] = 0.978241474;
        output[20] = 0.978267496;
    } else {
        output[0] = 0;
        output[1] = 0.72505691;
        output[2] = 0.844465522;
        output[3] = 0.845466186;
        output[4] = 0.851517595;
        output[5] = 0.859254956;
        output[6] = 0.862496443;
        output[7] = 0.899248316;
        output[8] = 0.899452243;
        output[9] = 0.900194442;
        output[10] = 0.900244238;
        output[11] = 0.900298776;
        output[12] = 0.910843688;
        output[13] = 0.910879256;
        output[14] = 0.910912454;
        output[15] = 0.978537892;
        output[16] = 0.979818363;
        output[17] = 0.98124585;
        output[18] = 0.981426065;
        output[19] = 0.981497202;
        output[20] = 0.981855259;
    }

    // Wicket probability
    double wkt_value = 1 - prob_wkt(bat, bowl, {});
    output[21] = wkt_value;

    // Rescale fixed probabilities
    for (int i = 0; i < NUM_DELIV_OUTCOMES - 1; i++) {
        output[i] = output[i] * wkt_value;
    }

    return output;
}

DismType MODEL_WICKET_TYPE(BowlType bowltype) {
    // This desperately needs cleaning up

    DismType* DISM_MODES = new DismType[NUM_DISM_MODES];
    double* DISM_MODE_DIST = new double[NUM_DISM_MODES];

    double DISM_MODE_SPINNER[6] = {0, 0.157, 0.692, 0.7274, 0.9286, 0.9613};
    double DISM_MODE_SEAMER[6] = {0, 0.175, 0.815, 0.8291, 0.9731, 1};

    bool is_spinner = is_slow_bowler(bowltype);

    // Check if "f" is in bowl_type - indicates whether stumpings are possible
    for (int i = 0; i < NUM_DISM_MODES; i++) {
        DISM_MODES[i] = DISM_MODES_STATIC[i];
        if (is_spinner) {
            // Spinner model
            // double DISM_MODE_DIST [8] = {0.157, 0.534, 0.0359, 0.201, 0.0326,
            // 0.0387};
            DISM_MODE_DIST[i] = DISM_MODE_SPINNER[i];
        } else {
            // Seamer model
            // double DISM_MODE_DIST [8] = {0.175, 0.640, 0.0141, 0.144, 0.0242,
            // 0};
            DISM_MODE_DIST[i] = DISM_MODE_SEAMER[i];
        }
    }

    // Sample from distribution
    DismType dism_mode =
        sample_cdf<DismType>(DISM_MODES, NUM_DISM_MODES, DISM_MODE_DIST);

    delete[] DISM_MODE_DIST;
    delete[] DISM_MODES;
    return dism_mode;
}
} // namespace Model
