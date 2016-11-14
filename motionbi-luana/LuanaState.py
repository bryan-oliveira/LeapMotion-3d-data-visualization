
class LuanaState():

    def __init__(self, ):
        self.registry = []
        self.prevstate = {'left' : None, 'right' : None, 'both' : None}

    def addGesture(self, gesture):
        self.registry.append(gesture)

    def evalCurrentState( self, history ):

        counts = [[ gesture.eval_state(state) for state in history ].count(True) for gesture in self.registry]

        newstate = {'nleft':0, 'left':None, 'nright':0, 'right':None, 'nboth':0, 'both':None}
        for i in range(len(counts)):
            if( counts[i] > len(history)/2 ):
                if( self.registry[i].hand == 'left' and counts[i] > newstate['nleft'] ):
                    newstate['left'] = self.registry[i]
                elif( self.registry[i].hand == 'right' and counts[i] > newstate['nright'] ):
                    newstate['right'] = self.registry[i]
                elif( self.registry[i].hand == 'both' and counts[i] > newstate['nboth'] ):
                    newstate['both'] = self.registry[i]

        if( newstate['left'] != None and newstate['left'] != self.prevstate['left'] ):
            newstate['left'].on_start()

        if( newstate['right'] != None and newstate['right'] != self.prevstate['right'] ):
            newstate['right'].on_start()

        if( newstate['both'] != None and newstate['both'] != self.prevstate['both'] ):
            newstate['both'].on_start()

        self.prevstate = newstate

        return newstate


