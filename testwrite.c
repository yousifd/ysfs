#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int res;
	int i;
	int temp;
	
	struct stat tbuf;

        int fd;
        fd = open(argv[3], O_WRONLY);
        
        fstat(fd, &tbuf);
	int size = tbuf.st_size;
	
	//printf("size: %d\n", tbuf.st_size);

	int one,two,thr;

	one = atoi(argv[1]);
	two = atoi(argv[2]);
	thr = atoi(argv[4]);

//	printf("argv[1] = %d\n", one);
//	printf("argv[2] = %d\n", two);
//	printf("argv[3] = %s\n", argv[3]);

        char *buf;
        buf = calloc(one + 1, sizeof(char));
	memset(buf, 'A', one);

	temp = two;
	for (i = 0; i < thr; i++)
	{
	if (((unsigned int)(two + temp)) > size)
	{
		//printf("SET TO ZERO\n");
		two = (i * 16);
	}
        res = pwrite(fd, buf, one, two += temp);
        if (res == -1)
        {
        	perror("Error occured: ");
        	return -errno;
        }
        //fsync(fd);
        }

	close(fd);
	//fflush(stdout);
        return 0;
}

