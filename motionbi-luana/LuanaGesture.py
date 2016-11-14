import Leap
from LuanaState import *
import main
from main import send, sendds, sendwr, sendgv
import time
import math

viz_mode = {'left' : 1, 'right' : 1}

class LuanaGesture():

    def __init__( self, name, hand ):
        self.name = name
        self.hand = hand

    def on_start(self):
        raise NotImplementedError("No start function defined for %s" % self)

    def eval_state(self, state):
        raise NotImplementedError("No function to evaluate states defined for %s" % self)

    def handle(self, hands):
        raise NotImplementedError("No handler defined for %s" % self)

    def __unicode__(self):
        return 'LUANA Gesture: %s' % self.name


class CameraMoveGesture(LuanaGesture):

    def __init__( self, hand ):
        self.name = 'Camera Move'
        self.hand = hand
        self.yoffset = -100

    def on_start(self):
        pass

    def eval_state(self, state):
        if( state.has_key(self.hand) and state[self.hand]['nfingers'] >= 4 ):
            return True
        return False

    def handle(self, hands):
        
        if( not self.eval_state(hands[0]) ):
            return

        minx, maxx = -200, 200
        miny, maxy =    0, 200
        minz, maxz = -200, 200
        
        normx = min( max(minx, hands[0][self.hand]['x'])               , maxx ) / float(maxx-minx) * 2
        normy = min( max(miny, hands[0][self.hand]['y']) + self.yoffset, maxy ) / float(maxy-miny) * 2
        normz = min( max(minz, hands[0][self.hand]['z'])               , maxz ) / float(maxz-minz) * 2
        
        send(type=1, how=1, angle=0.0, distance=0.0, x=normx, y=normy, z=normz)


class GraphMergeGesture(LuanaGesture):

    def __init__(self):
        self.name = 'Graph Merge'
        self.hand = 'both'

    def on_start(self):
        print self.operation + ' operation on the graphs'

        if(self.operation == 'split'):
            sendgv( type = 7, visibility = 1, graph = 0 )
            sendgv( type = 7, visibility = 1, graph = 1 )
            sendgv( type = 7, visibility = 0, graph = 2 )
            main.merged = False
        else:
            sendgv( type = 7, visibility = 0, graph = 0 )
            sendgv( type = 7, visibility = 0, graph = 1 )
            sendgv( type = 7, visibility = 1, graph = 2 )
            main.merged = True

    def eval_state(self, state):
        
        left_circle  = None
        right_circle = None
        rc = lc = False

        if( state.has_key('left') and state.has_key('right') ):
            
            for g in state['left']['gestures']:
                if(g.type == Leap.Gesture.TYPE_CIRCLE):
                    left_circle = Leap.CircleGesture(g)
                    lc = True
                    break
            
            for g in state['right']['gestures']:
                if(g.type == Leap.Gesture.TYPE_CIRCLE):
                    right_circle = Leap.CircleGesture(g)
                    rc = True
                    break

            if( lc and rc ):

                if( left_circle.pointable.direction.angle_to(left_circle.normal) <= Leap.PI/4.0 ):
                    left_dir = 'cw'
                else:
                    left_dir = 'ccw'

                if( right_circle.pointable.direction.angle_to(right_circle.normal) <= Leap.PI/4.0 ):
                    right_dir = 'cw'
                else:
                    right_dir = 'ccw'

                if(left_dir != right_dir):
                    if(left_dir == 'cw'):
                        self.operation = 'merge'
                    else:
                        self.operation = 'split'
                    return True

        return False

    def handle(self, hands):
        pass


class GraphRotationGesture(LuanaGesture):

    def __init__(self, hand):
        self.name = 'Graph Rotation'
        self.hand = hand

    def on_start(self):
        pass

    def eval_state(self, state):
        if( state.has_key(self.hand) and state[self.hand]['nfingers'] == 3 ):
            return True
        return False

    def handle(self, hands):

        if( not ( self.eval_state(hands[0]) and self.eval_state(hands[1]) ) ):
            return

        angle = (hands[0][self.hand]['x'] - hands[1][self.hand]['x']) / 360 * math.pi
        graph = 0 if self.hand == 'left' else 1

        sendwr(type=6, how=0, angle=angle, x=0, y=1, z=0, graph= 0 if self.hand == 'left' else 1 )


class ModeSelectionGesture(LuanaGesture):

    def __init__(self, hand):
        self.name = 'Visualization Mode Selection'
        self.hand = hand
        self.last_rotate_ev = time.time()

    def on_start(self):
        self.progress = 0
        self.prev_progress = 0

    def eval_state(self, state):
        if( state.has_key(self.hand) and any([g.type == Leap.Gesture.TYPE_CIRCLE for g in state[self.hand]['gestures']])):
            return True
        return False

    def handle(self, hands):
        
        global viz_mode

        if( not self.eval_state(hands[0]) ):
            return

        for gesture in hands[0][self.hand]['gestures']:
            if( gesture.type == Leap.Gesture.TYPE_CIRCLE ):

                circle = Leap.CircleGesture(gesture)

                if circle.pointable.direction.angle_to(circle.normal) <= Leap.PI/4.0:
                    clockwise = True
                else:
                    clockwise = False

                if circle.state != Leap.Gesture.STATE_START:
                    
                    self.progress += circle.progress - self.prev_progress
                    self.prev_progress = circle.progress

                    if( self.progress > 1.0 ):

                        if( clockwise ):
                            viz_mode[self.hand] = viz_mode[self.hand] % 4 + 1
                            if(viz_mode[self.hand] == 3):
                                viz_mode[self.hand] += 1
                        else:
                            viz_mode[self.hand] = (viz_mode[self.hand] - 2) % 4 + 1
                            if(viz_mode[self.hand] == 3):
                                viz_mode[self.hand] -= 1

                        sendds(type = 5, mode = viz_mode[self.hand], hdir=0, ddir = 0, graph = 0 if self.hand == 'left' else 1)
                        print('visualization mode changed')
                        self.progress %= 1.0

                else:
                    self.prev_progress = circle.progress


class BarSelectionGesture(LuanaGesture):

    def __init__(self, hand):
        self.name = 'Bar Selection'
        self.hand = hand
        self.threshold = 30

    def on_start(self):
        self.acum = { 'horiz': 0, 'depth': 0 }

    def eval_state(self, state):
        if( state.has_key(self.hand) and state[self.hand]['nfingers'] == 2 ):
            return True
        return False

    def send_depth(self, mode, graph):
        while abs(self.acum['depth']) >= self.threshold:
            sendds(type=5, mode=mode, hdir=0, ddir = -1 if self.acum['depth'] > 0 else 1, graph = graph)
            if( self.acum['depth'] > 0 ):
                self.acum['depth'] -= self.threshold
            else:
                self.acum['depth'] += self.threshold

    def send_horiz(self, mode, graph):
        while abs(self.acum['horiz']) >= self.threshold:
            sendds(type=5, mode=mode, hdir=1 if self.acum['horiz'] > 0 else -1, ddir = 0, graph = graph)
            if( self.acum['horiz'] > 0 ):
                self.acum['horiz'] -= self.threshold
            else:
                self.acum['horiz'] += self.threshold

    def handle(self, hands):

        global viz_mode

        if( not ( self.eval_state(hands[0]) and self.eval_state(hands[1]) ) ):
            return

        index = 0 if self.hand == 'left' else 1

        #Depth Selection
        depth_progress = hands[0][self.hand]['z'] - hands[1][self.hand]['z']
        horiz_progress = hands[0][self.hand]['x'] - hands[1][self.hand]['x']

        self.acum['depth'] += depth_progress
        self.acum['horiz'] += horiz_progress

        if( 1 < viz_mode[self.hand] < 4 ):

            if( abs(depth_progress) > abs(horiz_progress) ):
                self.send_depth(3, index)
                self.acum['horiz'] = 0
            else:
                self.send_horiz(2, index)
                self.acum['depth'] = 0
        elif( viz_mode[self.hand] == 4 ):
            self.send_depth(4, index)
            self.send_horiz(4, index)




