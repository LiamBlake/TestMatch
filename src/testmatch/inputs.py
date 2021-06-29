""" Defines all dataclasses which are provided to the simulation.

These mirror many of the structs defined in the core C++ library."""

from __future__ import annotations

import json
from dataclasses import dataclass
from typing import List, Optional

from dataclasses_json import dataclass_json

from ._base import cppable
from ._testmatch import _PitchFactors, _Player, _Stats, _Team, _Venue
from .enums import Arm, BowlType


def jsoninput(cls=None, **kwargs):
    def wrapper(cls):
        @classmethod
        def dump(cls, obj, file: str):
            jsoned = obj.to_dict()
            with open(file, "w") as fp:
                json.dump(jsoned, fp=fp, indent=4)

        @classmethod
        def load(cls, file: str):
            with open(file) as fp:
                jsoned = json.load(fp)
            return cls.from_dict(jsoned)

        # Apply dataclass decorators
        cls = dataclass(cls, **kwargs)
        cls = dataclass_json(cls)

        # Set dump and load attributes
        setattr(cls, "dump", dump)
        setattr(cls, "load", load)

        return cls

    if cls is None:
        return wrapper
    else:
        return wrapper(cls)


@cppable(cpp_rep=_Stats)
@jsoninput
class Stats:

    innings: int
    bat_avg: Optional[float]
    bat_sr: Optional[float]

    balls_bowled: int
    bowl_avg: Optional[float]
    bowl_sr: Optional[float]
    bowl_econ: float

    bat_arm: Arm = Arm.right
    bowl_arm: Arm = Arm.right
    bowl_type: BowlType = BowlType.med


@cppable(cpp_rep=_Player)
@jsoninput(frozen=True)
class Player:

    first_name: str
    last_name: str
    initials: str

    stats: Stats

    @property
    def full_name(self) -> str:
        return f"{self.first_name} {self.last_name}"

    @property
    def full_initials(self) -> str:
        return f"{self.initials} {self.last_name}"


@cppable(cpp_rep=_Team)
@jsoninput
class Team:
    name: str
    players: List[Player]

    idx_captain: int
    idx_wk: int
    idx_bowl1: int
    idx_bowl2: int

    def __str__(self) -> str:
        output = f"{self.name}\n"
        for i, p in enumerate(self.players):
            output += f"{i}. {p.full_name}"
            if i == self.idx_captain:
                output += " (c)"
            if i == self.idx_wk:
                output += " (wk)"
            output += "\n"
        return output


@cppable(cpp_rep=_PitchFactors)
@jsoninput
class PitchFactors:
    seam: float
    spin: float


@cppable(cpp_rep=_Venue)
@jsoninput(frozen=True)
class Venue:
    name: str
    city: str
    country: str
    pitch: PitchFactors
