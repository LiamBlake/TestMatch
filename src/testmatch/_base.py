from __future__ import annotations

from enum import Enum
from typing import Type

# To resolve missing definitions
from ._testmatch import _Arm, _BowlType  # noqa: F401


def cppable(cpp_rep: Type):
    def wrapper(cls):
        @classmethod
        def from_cpp(cls, cpp_obj):
            pass

        @property
        def cpp(self):
            fields = self.__dict__
            # Recursively convert any cppable members to cpp
            for key, value in fields.items():
                # TODO: This needs to be reconsidered
                # Enums are special
                if isinstance(value, Enum):
                    print(value, key, fields[key])
                    fields[key] = eval(f"_{type(value).__name__}").__members__[value]
                # Some are stored as lists
                elif isinstance(value, list):
                    fields[key] = [v.cpp if hasattr(v, "cpp") else v for v in value]
                elif hasattr(value, "cpp"):
                    fields[key] = value.cpp

            return self._cpp_rep(*fields.values())

        setattr(cls, "_cpp_rep", cpp_rep)
        setattr(cls, "from_cpp", from_cpp)
        setattr(cls, "cpp", cpp)

        return cls

    return wrapper
