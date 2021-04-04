'''
these algorithms are from Astronomical algorithms by Jean Meeus
'''


from datetime import datetime
from math import cos, pi

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

def calculate_anomalys_and_corrections(seconds_since_epoch):

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

    # eccentricity of earth's orbit around the sun correction
    E = 1 - 0.002516*T - 0.0000074*(T**2)

    return D, M, M_prime, F, E

    
def cosine_in_degrees(argument_in_degrees):

    return cos((argument_in_degrees) * (pi / 180))

def calculate_moon_distance(seconds_since_epoch):
    ''' returns the distance to the moon in kilometers seems to be good to +=20km '''

    D, M, M_prime, F, E = calculate_anomalys_and_corrections(seconds_since_epoch)

    # eccentricity correction for terms containing (M or -M) (E) or (2M or -2M) (E^2)
    
    # calcuate the sum of the terms in the table before
    sum_r = 0

    # (a,b,c,d,e) expands to e*cos(a*D + b*M + c*M_prime + d*F)
    D_coefficients = [0, 2, 2, 0, 0, 0, 2, 2, 2, 2, 0, 1, 0, 2, 0, 0, 4, 0, 4, 2, 2, 1, 1, 2, 2, 4, 2, 0, 2, 2, 1, 2] + [0, 0, 2, 2, 2, 4, 0, 3, 2, 4, 0, 2, 2, 2, 4, 0, 4, 1, 2, 0, 1, 3, 4, 2, 0, 1, 2, 2]
    M_coefficients = [0, 0, 0, 0, 1, 0, 0, -1, 0, -1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, -1, 0, 0, 0, 1, 0, -1, 0, -2] + [1, 2, -2, 0, 0, -1, 0, 0, 1, -1, 2, 2, 1, -1, 0, 0, -1, 0, 1, 0, 1, 0, 0, -1, 2, 1, 0, 0]
    M_prime_coefficients = [1, -1, 0, 2, 0, 0, -2, -1, 1, 0, -1, 0, 1, 0, 1, 1, -1, 3, -2, 1, 0, -1, 0, 1, 2, 0, -3, -2, -1, -2, 1, 0] + [2, 0, -1, 1, 0, -1, 2, -1, 1, -2, -1, -1, -2, 0, 1, 4, 0, -2, 0, 2, 1, -2, -3, 2, 1, -1, 3, -1]
    F_coefficients = [0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, -2, 2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0] + [0, 0, 0, -2, 2, 0, 2, 0, 0, 0, 0, 0, 0, -2, 0, 0, 0, 0, -2, -2, 0, 0, 0, 0, 0, 0, 0, -2]
    cos_coefficients = [-20905355, -3699111, -2955968, -569925, 48888, -3149, 246158, -152138, -170733, -204586, -129620, 108743, 104755, 10321, 0, 79661, -34782, -23210, -21636, 24208, 30824, -8379, -16675, -12831, -10445, -11650, 14403, -7003, 0, 10056, 6322, -9884] + [5751, 0, -4950, 4130, 0, -3958, 0, 3258, 2616, -1897, -2117, 2354, 0, 0, -1423, -1117, -1571, -1739, 0, -4421, 0, 0, 0, 0, 1165, 0, 0, 8752]
    for i in range(len(cos_coefficients)):

        new_term = cos_coefficients[i]*cosine_in_degrees(
                                        D_coefficients[i]*D + 
                                        M_coefficients[i]*M + 
                                        M_prime_coefficients[i]*M_prime +
                                        F_coefficients[i]*F)

        # perform the eccentricity correction
        if M_coefficients[i] in [1, -1]:
            new_term *= E
        elif M_coefficients[i] in [2, -2]:
            new_term *= E**2

        sum_r += new_term

    distance = 385_000.56 + sum_r/1000

    # the 
    return int(distance)
# example from the book
print(calculate_moon_distance(703036800))

print(calculate_moon_distance(1617538558))
print(calculate_moon_distance(1617544199))