events = []
#We need to keep track of the most recent event and ensure that two initialtile events don't occur in sequence.
values = []

'''
This function will return either the latest event from this list of events, or end the game immediately if GAMEOVER is on the list. 
Or return a flag that tells us if it's an empty list.
'''
LastEvent = "none"

def get(events, values):
    event = events[0]
    events = events.pop(0)
    value = values[0]
    values = values.pop(0)
    return event, value