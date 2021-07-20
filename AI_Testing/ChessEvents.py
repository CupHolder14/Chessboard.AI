events = []

'''
This function will return either the latest event from this list of events, or end the game immediately if GAMEOVER is on the list. 
Or return a flag that tells us if it's an empty list.
'''
def get(events):
    if events:
        event = events[0]
        events = events.pop(0)
    return event
