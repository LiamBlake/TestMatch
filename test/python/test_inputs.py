from pytest import fixture, mark
from testmatch import Player, Stats


@fixture
def stats():
    yield Stats(
        innings=65,
        bat_avg=16.46,
        bat_sr=37.44,
        balls_bowled=7734,
        bowl_avg=21.59,
        bowl_sr=47.10,
        bowl_econ=2.74,
        bat_arm="right",
        bowl_arm="right",
        bowl_type="fm",
    )


@fixture
def player(stats):
    yield Player(first_name="Pat", last_name="Cummins", initials="PJ", stats=stats)


class TestPlayer:
    def test_names(self, player):
        """
        GIVEN: A Player object.
        WHEN: Getting a formatted name with full_name or full_initials.
        THEN: The names are formatted correctly.
        """

        assert player.full_name == "Pat Cummins"
        assert player.full_initials == "PJ Cummins"


@mark.parametrize("fixture", ["stats1", "player1", "venue"])
class TestCppReps:
    def test_to_cpp(self, fixture, request):
        obj = request.getfixturevalue(fixture)

        cpp_obj = obj.cpp

        # TODO: Check members
        assert cpp_obj
