# -*- coding: utf-8 -*-

"""
    gcd(a, b) = gcd(b, a mod b)

    d = gcd(a, b)
    ax + by = d
    
    d|a and d|b implies d | (ax + by)
"""

import sys


def ext_euclid(a, b):
    if b == 0:
        return (a, 1, 0)
    else:
        print "a =", a, "b =", b
        d1, x1, y1 = ext_euclid(b, a % b)
        print "========= "
        print "a =", a, "b =", b
        d0 = d1
        x0 = y1
        y0 = x1 - a / b * y1
        print "d1 =", d1, "x1 =", x1, "y1 =", y1
        print "d0 =", d0, "x0 =", x0, "y0 =", y0

    return (d0, x0, y0)


def main(argv):
    if len(argv) < 3:
        print "ext_euclid num1 num2"
        return

    d, x, y = ext_euclid(int(argv[1]), int(argv[2]))
    print "========= "
    print "linear combination:", d, x, y


if __name__ == '__main__':
    main(sys.argv)
