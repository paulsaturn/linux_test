#!/usr/bin/python
# python gr.py

import greenlet

def run(name, nextGreenlets):
	for i in xrange(10):
		print name
	if nextGreenlets:
		nextGreenlets.pop(0).switch(chr(ord(name) + 1), nextGreenlets)

greenletA = greenlet.greenlet(run)
greenletB = greenlet.greenlet(run)

greenletA.switch('A', [greenletB])
