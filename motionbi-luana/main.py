#!/usr/bin/env python2
# -*- coding: utf-8 -*-
# LUANA engine

import time
import Leap
import socket
import struct
import math
import os
import math

from LuanaGesture import *
from LuanaState import *

merged = False
udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
host, port = "127.0.0.1", 12345

def send(type, how, angle, distance, x, y, z):
    packet = struct.pack('iifffff', type, how, angle, distance, x, y, z)
    udpsock.sendto(packet, (host, port))

def sendds(type, mode, hdir, ddir, graph):
    if(merged):
        packet = struct.pack('iiiii', type, mode, hdir, ddir, 2)
    else:
        packet = struct.pack('iiiii', type, mode, hdir, ddir, graph)
    udpsock.sendto(packet, (host, port))

def sendgv(type, visibility, graph):
    packet = struct.pack('iii', type, visibility, graph)
    udpsock.sendto(packet, (host, port))

def sendwr(type, how, angle, x, y, z, graph):
    if(merged):
        packet = struct.pack('iiffffi', type, how, angle, x, y, z, 2)
    else:
        packet = struct.pack('iiffffi', type, how, angle, x, y, z, graph)
    udpsock.sendto(packet, (host, port))


class Listener(Leap.Listener):

    def on_init(self, controller):
        print "Initialized"
        self.luanaState = LuanaState()
        self.luanaState.addGesture(CameraMoveGesture(hand = 'left'))
        self.luanaState.addGesture(GraphRotationGesture(hand = 'left'))
        self.luanaState.addGesture(ModeSelectionGesture(hand = 'left'))
        self.luanaState.addGesture(BarSelectionGesture(hand = 'left'))
        self.luanaState.addGesture(CameraMoveGesture(hand = 'right'))
        self.luanaState.addGesture(GraphRotationGesture(hand = 'right'))
        self.luanaState.addGesture(ModeSelectionGesture(hand = 'right'))
        self.luanaState.addGesture(BarSelectionGesture(hand = 'right'))
        self.luanaState.addGesture(GraphMergeGesture())

    def on_connect(self, controller):
        print "Connected"
        controller.enable_gesture(Leap.Gesture.TYPE_CIRCLE)
        controller.enable_gesture(Leap.Gesture.TYPE_KEY_TAP)
        controller.enable_gesture(Leap.Gesture.TYPE_SCREEN_TAP)
        controller.enable_gesture(Leap.Gesture.TYPE_SWIPE)

    def on_disconnect(self, controller):
        print "Disconnected"

    def on_frame(self, controller):

        history_size = 8

        frames = [controller.frame(i) for i in range(history_size)]

        if( all( [all( [h.is_valid for h in frame.hands ] ) for frame in frames ] ) ):

            hands = []
            for frame in frames:

                #for noise reduction, we consider the palm position with only a few
                #decimal numbers of precision.
                palms = [ {'x'       : int(h.palm_position[0]*100)/100.0,
                           'y'       : int(h.palm_position[1]*100)/100.0,
                           'z'       : int(h.palm_position[2]*100)/100.0,
                           'nfingers': len(h.fingers),
                           'hand'    : h,
                           'gestures': [ g for g in frame.gestures() if h in g.hands ],
                          } for h in frame.hands ]

                try:
                    lefthand  = min( palms, key = lambda val : val['x'] )
                    righthand = max( palms, key = lambda val : val['x'] )

                    if(lefthand != righthand):
                        fhands = { 'nhands' : 2,
                                   'left'   : lefthand,
                                   'right'  : righthand }
                    else:
                        fhands = { 'nhands' : 1,
                                   'right'   : righthand }

                except ValueError:
                    fhands = {'nhands' : 0}
                    return

                hands.append(fhands)

            handlers = self.luanaState.evalCurrentState(hands)

            #Look for conflits and stuff.
            if( handlers['left'] != None and handlers['right'] != None ):
                if( type(handlers['left']) == type(handlers['right']) and type(handlers['left']) == CameraMoveGesture ):
                    handlers['right'].handle(hands)
                    return

            if(handlers['both'] != None):
                handlers['both'].handle(hands)
            else:
                if(handlers['left'] != None):
                    handlers['left'].handle(hands)
                
                if(handlers['right'] != None):
                    handlers['right'].handle(hands)


if __name__ == '__main__':

    listener = Listener()
    controller = Leap.Controller()
    controller.set_policy_flags(Leap.Controller.POLICY_BACKGROUND_FRAMES)
    controller.add_listener(listener)
    os.system("../avi/bin/gprs")
    controller.remove_listener(listener)

