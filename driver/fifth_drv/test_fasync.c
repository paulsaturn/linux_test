#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
 
 
/* fifthdrvtest 
  */
int fd;
 
//信号处理函数
void my_signal_fun(int signum)
{
	unsigned char key_val;
	read(fd, &key_val, 1);
	printf("key_val: 0x%x\n", key_val);
}
 
int main(int argc, char **argv)
{
	unsigned char key_val;
	int ret;
	int Oflags;
	int cnt = 20;
 
	//在应用程序中捕捉SIGIO信号（由驱动程序发送）
	signal(SIGIO, my_signal_fun);
	
	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
		return -1;
	}
	
	printf("start....\n");
 
	//将当前进程PID设置为fd文件所对应驱动程序将要发送SIGIO,SIGUSR信号进程PID
	fcntl(fd, F_SETOWN, getpid());
	
	//获取fd的打开方式
	Oflags = fcntl(fd, F_GETFL); 
 
	//将fd的打开方式设置为FASYNC --- 即 支持异步通知
	//该行代码执行会触发 驱动程序中 file_operations->fasync 函数 ------fasync函数调用fasync_helper初始化一个fasync_struct结构体，该结构体描述了将要发送信号的进程PID (fasync_struct->fa_file->f_owner->pid)
	fcntl(fd, F_SETFL, Oflags | FASYNC);
 
	while (cnt--) {
		sleep(1000);
	}
	printf("end....\n");
	
	close(fd);
	
	return 0;
}
