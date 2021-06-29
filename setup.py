from setuptools import find_packages
from skbuild import setup

setup(
    name="testmatch",
    url="https://github.com/LiamBlake/TestMatch",
    author="Liam Blake",
    author_email="",
    packages=find_packages("src"),
    package_dir={"": "src"},
    install_requires=["dataclasses-json==0.5.4"],
    extras_require={
        "dev": [
            "black==21.6b0",
            "flake8==3.9.2",
            "pytest==6.2.4",
            "pytest-cov==2.12.1",
            "isort==5.9.1",
            "mypy==0.910",
        ]
    },
    cmake_args=[
        "-DBUILD_AS_PYTHON=ON",
        "-GUnix Makefiles",
    ],
    cmake_install_dir="src/testmatch",
    include_package_data=True,
    package_data={"": ["libTestMatch.so"]},
    version="0.1",
    license="GNU",
    description="Cricket simulation built on historical data.",
    long_description=open("README.md").read(),
)
