'''
gevent的真正威力是在处理网络和带有IO阻塞的功能时能够这些任务协调地运行。gevent来实现了这些具体的细节来保证在需要的时候greenlet上下文进行切换
输出结果:
Started Polling:  at 0.0 seconds
Started Polling:  at 0.0 seconds
Hey lets do some stuff while the greenlets poll, at at 0.0 seconds
Ended Polling:  at 2.0 seconds
Ended Polling:  at 2.0 seconds


初始化的greenlet放在了threads这个list里面，被传递给了 gevent.joinall 这个函数，它会阻塞当前的程序来执行所有的greenlet
'''
import time
import gevent
from gevent import select

start = time.time()
tic = lambda: 'at %1.1f seconds' % (time.time() - start)

def gr1():
    # Busy waits for a second, but we don't want to stick around...
    print('Started Polling: ', tic())
    select.select([], [], [], 2)
    print('Ended Polling: ', tic())

def gr2():
    # Busy waits for a second, but we don't want to stick around...
    print('Started Polling: ', tic())
    select.select([], [], [], 2)
    print('Ended Polling: ', tic())

def gr3():
    print("Hey lets do some stuff while the greenlets poll, at", tic())
    gevent.sleep(1)

gevent.joinall([
    gevent.spawn(gr1),
    gevent.spawn(gr2),
    gevent.spawn(gr3),
])
