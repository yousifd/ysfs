//Yousif ALDOLAIJAN
//YSFS: Yousif's Secure File System
//	gcc -Wall  ysfs.c -o ysfs `pkg-config fuse --cflags --libs`

#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

/*void* ysfs_init(struct fuse_conn_info *conn)
{

}*/

static int ysfs_getattr(const char* path, struct stat* stbuf)
{
	int res;

	res = lstat(path, stbuf);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}

	return 0;
}

static int ysfs_access(const char *path, int mask)
{
	int res;
	
	res = access(path, mask);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}

static int ysfs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)
{
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(path);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int ysfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;

	res = mknod(path, mode, rdev);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}

	return 0;
}

static int ysfs_mkdir(const char* path, mode_t mode)
{
	int res;
	
	res = mkdir(path, mode);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}

static int ysfs_rmdir(const char* path)
{
	int res;
	
	res = rmdir(path);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}

static int ysfs_chmod(const char* path, mode_t mode)
{
	int res;
	
	res = chmod(path, mode);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}

static int ysfs_chown(const char* path, uid_t uid, gid_t gid)
{
	int res;
	
	res = lchown(path, uid, gid);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}

static int ysfs_open(const char* path, struct fuse_file_info* fi)
{
	int res;
	
	res = open(path, fi->flags);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	close(res);
	return 0;
	
}

static int ysfs_read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
	int fd;
	int res;
	
	(void) fi;
	
	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	res = pread(fd, buf, size, offset);
	if (res == -1)
	{
		perror("The Following Error Occured");
		res = -errno;
	}
	
	close(fd);
	return res;
}

static int ysfs_write(const char* path,const char *buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
	int fd;
	int res;
	
	(void) fi;
	
	fd = open(path, O_WRONLY);
	if (fd == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	res = pwrite(fd, buf, size, offset);
	if (res == -1)
	{
		perror("The Following Error Occured");
		res = -errno;
	}
	
	close(fd);
	return res;
}

static int ysfs_readlink(const char* path, char* buf, size_t size)
{
	int res;
	
	res = readlink(path, buf, size);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}

static int ysfs_unlink(const char *path)
{
	int res;
	
	res = unlink(path);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}

static int ysfs_symlink(const char *to, const char *from)
{
	int res;
	
	res = symlink(from, to);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}

static int ysfs_rename(const char *from, const char *to)
{
	int res;
	
	res = rename(from, to);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}

static int ysfs_link(const char *from, const char *to)
{
	int res;
	
	res = link(from, to);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}

static int ysfs_truncate(const char *path, off_t size)
{
	int res;
	
	res = truncate(path, size);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}

static int ysfs_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(path, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int ysfs_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
	
	res = statvfs(path, stbuf);
	if (res == -1)
	{
		perror("The Following Error Occured");
		return -errno;
	}
	
	return 0;
}



static struct fuse_operations ysfs_oper = {
	//.init		= ysfs_init,
	.access		= ysfs_access,//DONE
	.getattr	= ysfs_getattr,//DONE
	.readdir	= ysfs_readdir,//DONE
	.mknod		= ysfs_mknod,//DONE
	.mkdir		= ysfs_mkdir,//DONE
	.rmdir		= ysfs_rmdir,//DONE
	.chmod		= ysfs_chmod,//DONE
	.chown		= ysfs_chown,//DONE
	.open		= ysfs_open,//DONE
	.read		= ysfs_read,//DONE
	.write		= ysfs_write,//DONE
	//START
	.readlink	= ysfs_readlink,//DONE
	.unlink		= ysfs_unlink,//DONE
	.symlink	= ysfs_symlink,//DONE
	.rename		= ysfs_rename,//DONE
	.link		= ysfs_link,//DONE
	.truncate	= ysfs_truncate,//DONE
	.utimens	= ysfs_utimens,//DONE
	.statfs		= ysfs_statfs,//DONE
	//END
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &ysfs_oper, NULL);
}
  //ARE THEY NEEDED?!?!
  /*int (*unlink) (const char *);
    int (*symlink) (const char *, const char *);
    int (*rename) (const char *, const char *);
    int (*link) (const char *, const char *);
    int (*truncate) (const char *, off_t);
    int (*utime) (const char *, struct utimbuf *);
    int (*statfs) (const char *, struct statfs *);*/


