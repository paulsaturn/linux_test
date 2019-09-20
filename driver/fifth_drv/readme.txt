1.
#编译驱动
make

#编译应用
make -f Makefile_test

2.
# 加载模块, 创建/dev/buttons
sudo insmod fifth_drv.ko
lsmod | grep fifth_drv
# 运行应用
sudo ./test_fasync

# 删除模块
sudo rmmod fifth_drv.ko
lsmod | grep fifth_drv

3.
应用层注册signal(SIGIO, my_signal_fun)接收信号SIGIO, 并设置FASYNC标志：
Oflags = fcntl(fd, F_GETFL); 
fcntl(fd, F_SETFL, Oflags | FASYNC);
其实调用驱动里的fifth_drv_fasync来绑定信号发送的进程

内核调用kill_fasync (&button_async, SIGIO, POLL_IN);
发送信号SIGIO的POLL_IN给注册的进程 
