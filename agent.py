#!/usr/env/python2
import tworld
import random
import time

def print_board(x_max=32,y_max=32):
    """note x_max and y_max are then number of tiles printed in that 
        direction"""
    for y in range(y_max):
        for x in range(x_max):
            print "%2x|" % tworld.get_tile(x,y)[0],
        print "\n",
        for x in range(x_max):
            print "%2x|" % tworld.get_tile(x,y)[1],
        print "\n",
    print "\n"

def verbose_agent():
    """print out lots of debug and then return a random direction"""
    print_board(9,9)
    x,y = tworld.chips_pos()
    print "Keys R:%d B:%d Y:%d G:%d" % tworld.get_keys()
    print "Boots Ice:%d Suction:%d Fire:%d Water:%d" % tworld.get_boots()
    print "(%d,%d)" % (x, y)
    print "Chips left: %d" % tworld.chips_needed()
    return random_move()

def random_move():
    """make random moves"""
    time.sleep(1)
    return random.choice( [tworld.NORTH, 
                           tworld.SOUTH, 
                           tworld.EAST, 
                           tworld.WEST,
                           tworld.WAIT])

tworld.set_agent( verbose_agent )
#tworld.load_level('cc-ms.dac',2)
tworld.load_level('classical.dac',3)
