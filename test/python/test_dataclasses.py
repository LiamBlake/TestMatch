from pytest import fixture


class TestPlayer:
    def test_names(self, player1):
        """
        GIVEN: A Player object.
        WHEN: Getting a formatted name with full_name or full_initials.
        THEN: The names are formatted correctly.
        """

        assert player1.full_name == "Pat Cummins"
        assert player1.full_initials == "PJ Cummins"
