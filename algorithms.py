'''
these algorithms are from Astronomical algorithms by Jean Meeus
'''


from datetime import datetime


def calculate_julian_ephemeris_day(seconds_since_epoch):

    # standard epoch J2000
    # the Julian day number for the day starting at 12:00 UT (noon) on January 1, 2000, was 2 451 545
    # this is equivalent to 946727936 in seconds since the unix epoch
    
    # each day is 60*60*24 seconds long

    JDE = (seconds_since_epoch - 946727936)/(60*60*24) + 2_451_545
    
    return JDE

def find_T_since_jealian_epoch(seconds_since_epoch):
    ''' this is expressed in fractional centuries which means that at least 9 decimal places should be taken '''
    
    JDE = calculate_julian_ephemeris_day(seconds_since_epoch)

    T = (JDE - 2_451_545)/36525

    return T

print(find_T_since_jealian_epoch(1617531924))