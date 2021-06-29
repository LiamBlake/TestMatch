from pytest import fixture
from testmatch import PitchFactors, Player, Stats, Team, Venue
from testmatch.enums import Arm, BowlType


# Fixtures
@fixture
def stats1():
    yield Stats(
        innings=65,
        bat_avg=16.46,
        bat_sr=37.44,
        balls_bowled=7734,
        bowl_avg=21.59,
        bowl_sr=47.10,
        bowl_econ=2.74,
        bat_arm=Arm.right,
        bowl_arm=Arm.right,
        bowl_type=BowlType.fast_med,
    )


@fixture
def player1(stats1):
    yield Player(first_name="Pat", last_name="Cummins", initials="PJ", stats=stats1)


@fixture
def team(player1):
    yield Team(
        name="WTC XI",
        players=[
            Player(
                first_name="Rohit",
                last_name="Sharma",
                initials="RG",
                stats=Stats(
                    innings=66,
                    bat_avg=46.18,
                    bat_sr=57.89,
                    balls_bowled=383,
                    bowl_avg=112,
                    bowl_sr=191.5,
                    bowl_econ=3.5,
                    bat_arm=Arm.right,
                    bowl_arm=Arm.right,
                    bowl_type=BowlType.offbreak,
                ),
            ),
            Player(
                first_name="Dimuth",
                last_name="Karunaratne",
                initials="Dimuth",
                stats=Stats(
                    innings=139,
                    bat_avg=38.62,
                    bat_sr=49.74,
                    balls_bowled=284,
                    bowl_avg=95.5,
                    bowl_sr=142.0,
                    bowl_econ=4.03,
                    bat_arm=Arm.right,
                    bowl_arm=Arm.right,
                    bowl_type=BowlType.med,
                ),
            ),
            Player(
                first_name="Marnus",
                last_name="Labuschagne",
                initials="M",
                stats=Stats(
                    innings=31,
                    bat_avg=60.8,
                    bat_sr=54.94,
                    balls_bowled=822,
                    bowl_avg=41.66,
                    bowl_sr=68.5,
                    bowl_econ=3.64,
                    bat_arm=Arm.right,
                    bowl_arm=Arm.right,
                    bowl_type=BowlType.legbreak,
                ),
            ),
            Player(
                first_name="Kane",
                last_name="Williamson",
                initials="KS",
                stats=Stats(
                    innings=148,
                    bat_avg=53.95,
                    bat_sr=51.76,
                    balls_bowled=2151,
                    bowl_avg=40.23,
                    bowl_sr=71.70,
                    bowl_econ=3.36,
                    bat_arm=Arm.right,
                    bowl_arm=Arm.right,
                    bowl_type=BowlType.offbreak,
                ),
            ),
            Player(
                first_name="Steven",
                last_name="Smith",
                initials="SPD",
                stats=Stats(
                    innings=139,
                    bat_avg=61.80,
                    bat_sr=55.11,
                    balls_bowled=1381,
                    bowl_avg=56.47,
                    bowl_sr=81.20,
                    bowl_econ=4.17,
                    bat_arm=Arm.right,
                    bowl_arm=Arm.right,
                    bowl_type=BowlType.legbreak,
                ),
            ),
            Player(
                first_name="Ben",
                last_name="Stokes",
                initials="BA",
                stats=Stats(
                    innings=130,
                    bat_avg=37.04,
                    bat_sr=58.12,
                    balls_bowled=9275,
                    bowl_avg=31.38,
                    bowl_sr=56.90,
                    bowl_econ=3.3,
                    bat_arm=Arm.left,
                    bowl_arm=Arm.right,
                    bowl_type=BowlType.fast_med,
                ),
            ),
            Player(
                first_name="Rishabh",
                last_name="Pant",
                initials="RR",
                stats=Stats(
                    innings=35,
                    bat_avg=43.84,
                    bat_sr=69.80,
                    balls_bowled=None,
                    bowl_avg=None,
                    bowl_sr=None,
                    bowl_econ=None,
                    bat_arm=Arm.left,
                ),
            ),
            Player(
                first_name="Ravichandran",
                last_name="Ashwin",
                initials="R",
                stats=Stats(
                    innings=111,
                    bat_avg=27.68,
                    bat_sr=53.76,
                    balls_bowled=21670,
                    bowl_avg=24.56,
                    bowl_sr=52.40,
                    bowl_econ=2.80,
                    bat_arm=Arm.right,
                    bowl_arm=Arm.right,
                    bowl_type=BowlType.offbreak,
                ),
            ),
            Player(
                first_name="Kyle",
                last_name="Jamieson",
                initials="RG",
                stats=Stats(
                    innings=8,
                    bat_avg=42.66,
                    bat_sr=71.70,
                    balls_bowled=1724,
                    bowl_avg=14.17,
                    bowl_sr=37.40,
                    bowl_econ=2.26,
                    bat_arm=Arm.right,
                    bowl_arm=Arm.right,
                    bowl_type=BowlType.fast_med,
                ),
            ),
            player1,
            Player(
                first_name="Tim",
                last_name="Southee",
                initials="TG",
                stats=Stats(
                    innings=111,
                    bat_avg=17.28,
                    bat_sr=84.74,
                    balls_bowled=17886,
                    bowl_avg=28.22,
                    bowl_sr=56.90,
                    bowl_econ=2.97,
                    bat_arm=Arm.right,
                    bowl_arm=Arm.right,
                    bowl_type=BowlType.med_fast,
                ),
            ),
        ],
        idx_captain=3,
        idx_wk=6,
        idx_bowl1=9,
        idx_bowl2=10,
    )


@fixture
def venue():
    yield Venue(
        name="Adelaide Oval",
        city="Adelaide",
        country="Australia",
        pitch=PitchFactors(0.5, 0.5),
    )
