# -*- coding: utf-8 -*-

from enum import Enum


class Orientation(Enum):
    R0 = 0
    MY = 1
    MX = 2
    R180 = 3
    MXR90 = 4
    R270 = 5
    R90 = 6
    MYR90 = 7


class PathStyle(Enum):
    TRUNCATE = 0
    EXTEND = 1
    ROUND = 2
    TRIANGLE = 3
