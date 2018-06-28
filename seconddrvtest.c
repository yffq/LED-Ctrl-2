
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

int main(void)
{
	int fd,err;
	void *start = NULL, *addr=NULL;
	fd = open("/dev/seconddrv", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}
	/*最少映射1页，32位系统中是1024*4；内存保护标志要与文件的打开模式一致*/
	/*最后一个参数是0x56000000可以，是0x56000050就不行*/
	start = mmap(NULL,1024*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x56000000);
	if(start == NULL)
	{
		printf("mmap err!\n");
		return -1;
	}
	addr=(void *)((char *)start+0x50);
	/* GPF4, 5, 6 correspond respectively LED1, LED2, LED4 */
	/* reset */ 
	*(unsigned *)addr &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));
	/* set, 0x01 means output */
	*(unsigned *)addr |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));
	addr=(void *)((char *)addr+0x04);
	/*0 corresponds lighted LED */
	/* 亮 */
	*(char *)addr &= ~ ((1<<4) | (1<<5) | (1<<6));
	sleep(0x01);
	/* 灭 */
	*(char *)addr |= (1<<4) | (1<<5) | (1<<6);
	sleep(0x01);
	/* 亮 */
	*(char *)addr &= ~ ((1<<4) | (1<<5) | (1<<6));
	sleep(0x01);
	/* 灭 */
	*(char *)addr |= (1<<4) | (1<<5) | (1<<6);
	munmap(start,1024*4);
	err=close(fd);
	if(err < 0)
		printf("can't close!\n");
	return 0;
}
