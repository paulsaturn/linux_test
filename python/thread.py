#!/usr/bin/python
# python thread.py
# python -m gevent.monkey thread.py

import threading

class Thread(threading.Thread):

	def __init__(self, name):
		threading.Thread.__init__(self)
		self.name = name

	def run(self):
		for i in xrange(10):
			print self.name

threadA = Thread("A")
threadB = Thread("B")

threadA.start()
threadB.start()
