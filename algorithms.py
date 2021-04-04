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

def find_T_since_julian_epoch(seconds_since_epoch):
    ''' this is expressed in fractional centuries which means that at least 9 decimal places should be taken '''
    
    JDE = calculate_julian_ephemeris_day(seconds_since_epoch)

    T = (JDE - 2_451_545)/36525

    return T

def calculate_moon_distance(seconds_since_epoch):

    T = find_T_since_julian_epoch(seconds_since_epoch)

    # these quantities are in degrees so reduce them to be
    # in the interval [0, 360)

    # mean elongation of the Moon
    D = 297.8501921 + 445267.1114034*T - 0.0018819*(T**2) + (T**3)/545868 - (T**4)/113065000
    D = D % 360
    
    # sun's mean anomaly
    M = 357.5291092 + 35999.0502909*T - 0.0001536*(T**2) + (T**3)/24490000
    M = M % 360

    # moon's mean anomaly
    M_prime = 134.9633964 + 477198.8675055*T + 0.0087414*(T**2) + (T**3)/69699 - (T**4)/14712000
    M_prime = M_prime % 360
    
    # moon's argument of latitude (mean distance of the moon from its ascending node)
    F = 93.2720950 + 483202.0175233*T - 0.0036539*(T**2) - (T**3)/3526000 + (T**4)/863310000
    F = F % 360

    return -1
print(calculate_moon_distance(703036800))
