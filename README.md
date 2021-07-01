# TestMatch

[![GitHub license](https://img.shields.io/github/license/LiamBlake/TestMatch.svg)](https://github.com/LiamBlake/TestMatch/blob/master/LICENSE)

[![code checks](https://github.com/LiamBlake/TestMatch/actions/workflows/checks.yml/badge.svg)](https://github.com/LiamBlake/TestMatch/actions/workflows/checks.yml) [![Documentation Status](https://readthedocs.org/projects/testmatch/badge/?version=latest)](https://testmatch.readthedocs.io/en/latest/?badge=latest) [![Codacy Badge](https://app.codacy.com/project/badge/Grade/f6f55f2a99bf40ceb541b5351616e77c)](https://www.codacy.com/gh/LiamBlake/TestMatch/dashboard?utm_source=github.com&utm_medium=referral&utm_content=LiamBlake/TestMatch&utm_campaign=Badge_Grade) [![codecov](https://codecov.io/gh/LiamBlake/TestMatch/branch/master/graph/badge.svg?token=E9O2ERHKT8)](https://codecov.io/gh/LiamBlake/TestMatch)

## Introduction

TestMatch is a cricket simulation library built in C++ and extended with Python. The core simulation is built in C++ and bound to the Python library using [Boost.Python](https://github.com/boostorg/python).

Full documentation is available [here](https://testmatch.readthedocs.io).

## Main Features
TBD


## The Simulation

The simulation itself consists of several different predictive models for different aspects of the game. The models are trained using ball-by-ball data from test matches from 2008 to present day. The tasks and models used (with links for a deeper explanation) in this library are as follows:

- Predicting when a wicket falls: TBD.

- Predicting the number of runs scored off a delivery: TBD.

- Deciding whether a declaration is made: TBD.

- Deciding whether to enforce the follow-on: TBD.

- Dynamically varying the batting order depending on game situation: TBD.

- Making bowling changes appropriate to the game situation: TBD.

## Installation

The full Python package ~~is~~ will be available on pip and installed via

```
pip install testmatch
```

The library can also be built entirely from source by building both the C++ core library and the Python bindings. This requires Boost.Python and is detailed below.

### C++ Library

The C++ library works as a standalone and dynamically-linked library, which is built using CMake. More details soon.

### Tests

The C++ library is tested using [Boost.Test](https://github.com/boostorg/test). More details soon.

### Python Bindings

More details soon.

## Contributing

This project is open-source, so any contributions are welcome via issues, discussion or pull requests.

Code style is maintained by [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) (for C++) and a combination of [flake8](https://github.com/PyCQA/flake8), [black](https://github.com/psf/black) and [isort](https://github.com/PyCQA/isort) (for Python), with customised rules defined in [.clang-format](https://github.com/LiamBlake/TestMatch/blob/master/setup.cfg) and [setup.cfg](https://github.com/LiamBlake/TestMatch/blob/master/.clang-format) respectively. These are automatically checked with Github actions, so please ensure that any code in a pull request follows these style requirements.

To develop the Python package, it is recommended that you use a virtual environment and setup the project from the root directory via

```
pip install -e . [dev]
```

More detail coming soon.

## Future Work

Some broad future goals for this project include:

- Extending to other formats of cricket, including men and women one-day matches and T20s.
