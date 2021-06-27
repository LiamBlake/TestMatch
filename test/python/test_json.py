from pytest import mark


@mark.parametrize("fixture", ["stats1"])
def test_json(fixture, request):
    obj = request.getfixturevalue(fixture)

    obj.dump(obj, f"testfiles/{fixture}.json")

    new = obj.load(f"testfiles/{fixture}.json")
    assert obj == new
