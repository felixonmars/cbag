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
    truncate = 0
    extend = 1
    round = 2
    triangle = 3


class BlockageType(Enum):
    routing = 0
    via = 1
    placement = 2
    wiring = 3
    fill = 4
    slot = 5
    pin = 6
    feed_thru = 7
    screen = 8


class BoundaryType(Enum):
    PR = 0
    snap = 1


class Orient2D(Enum):
    x = 0
    y = 1
