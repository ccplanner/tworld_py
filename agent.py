#!/usr/env/python2
import tworld
import random


def random_move():
    return random.choice( [tworld.NORTH, 
                           tworld.SOUTH, 
                           tworld.EAST, 
                           tworld.WEST,
                           tworld.WAIT])

tworld.set_agent( random_move )
tworld.load_level('sdfjkl',2)



