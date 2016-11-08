"""
Timing module detailed by 'Paul McGuire' of StackOverflow
http://stackoverflow.com/questions/1557571/how-to-get-time-of-a-python-program-execution/1557906#1557906
Updated to be Python3 compliant
"""

import atexit
from time import clock


def reduce(function, iterable, initializer=None):
    it = iter(iterable)
    if initializer is None:
        try:
            initializer = next(it)
        except StopIteration:
            raise TypeError('reduce() of empty sequence with no initial value')
    accum_value = initializer
    for x in it:
        accum_value = function(accum_value, x)
    return accum_value


def secondsToStr(t):
    return "%d:%02d:%02d.%03d" % \
        reduce(lambda ll,b : divmod(ll[0],b) + ll[1:],
            [(t*1000,),1000,60,60])


line = "="*40
def log(s, elapsed=None):
    print(line)
    print(secondsToStr(clock()), '-', s)
    if elapsed:
        print("Elapsed time:", elapsed)
    print(line)
    # print


def endlog():
    end = clock()
    elapsed = end-start
    log("End Program", secondsToStr(elapsed))


def now():
    return secondsToStr(clock())

start = clock()
atexit.register(endlog)
log("Start Program")
