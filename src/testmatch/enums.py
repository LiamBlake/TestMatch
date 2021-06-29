from enum import Enum


class Arm(str, Enum):
    left = "left"
    right = "right"


class BowlType(str, Enum):
    med = "med"
    med_fast = "med_fast"
    fast_med = "fast_med"
    fast = "fast"
    offbreak = "offbreak"
    legbreak = "legbreak"


class DismType(str, Enum):
    bowled = "bowled"
    lbw = "lbw"
    caught = "caught"
    c_and_b = "c_and_b"
    run_out = "run_out"
    stumped = "stumped"
