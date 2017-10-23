# coding=utf8

"""
	最大公约数
	Greatest Common Divisor by Euclid's algorithm.
	gcd(n, m) = gcd(m, n mod m)

	n = km+b => n%m = b = n-km
    1, if gcd(n,m) = r
        then n=pr and m=qr => n%m = pr-kqr = (p-kq)r
       So, r is common divisor of (m,n%m)

    2, if gcd(m,n%m) = r
        then m=pr and n%m=qr => n-km=qr => n=qr+km=qr+kpr=(q+kp)r
       So, r is common divisor of (n,m)

    Since 1 and 2, we know gcd(n,m) = gcd(m,n%m)
"""


def gcd(n, m):
    if n < m:
        t = m
        m = n
        n = t

    while m:
        r = n % m
        n = m
        m = r

    return n


def binary_gcd(a, b):
	if a == 0:
		return b

	if a % 2 == 1:
		if b % 2 == 1:
			# swap a and b
			if a < b:
				a = a ^ b
				b = b ^ a
				a = a ^ b

			return binary_gcd((a - b) / 2, b)
		else:
			return binary_gcd(a, b / 2)
	else:
		if b % 2 == 1:
			return binary_gcd(a / 2, b)
		else:
			return 2 * binary_gcd(a / 2, b / 2)


def gcd_many(items):
    g = items[0]

    for i in range(1, len(items)):
        g = gcd(items[i], g)

    return g


def lcm(n, m):
    """
    最小公倍数
    Least Common Multiple
    lcm(n,m) = n*m/gcd(n,m)
    """

    return n * (m / gcd(n, m))


def lcm_many(items):
	x = items[0]

	for i in range(1, len(items)):
		g = gcd(x, items[i])
		x = x * items[i] / g

	return x


if __name__ == '__main__':
    print "gcd = ", gcd(25, 30)   # 5
    print lcm(30, 25)   # 150
    print "gcd = ", gcd(23, 3)    # 1
    print lcm(3, 23)    # 69
    print "gcd_m = ", gcd_many([9, 12, 15])
    print lcm_many([9, 12, 15])
    print "gcd_m = ", gcd_many([3, 6, 9])
    print lcm_many([3, 6, 9])
    print "gcd_m = ", gcd_many([3, 4, 5])
    print lcm_many([3, 4, 5])
    print "gcd_m = ", gcd_many([3, 4, 6])
    print lcm_many([3, 4, 6])

    print "gcd_b = ", binary_gcd(25, 30)
    print "gcd_b = ", binary_gcd(3, 23)
    print "gcd_b = ", binary_gcd(6, 9)
