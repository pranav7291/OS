#include <kern/fcntl.h>



struct filedesc {
	struct vnode *fd_vnode;
	int fd_refcount;
	int fd_count;
	struct lock *fd_lock
	char name[100];
	int offset;
	int isempty;
	int flags;
	int read_count;
};

/**	added by sammokka
 O_RDONLY		Open for reading only.
 O_WRONLY		Open for writing only.
 O_RDWR		Open for reading and writing.
 */
int
sys_open(const char *filename, int flags, int *retval);


int
sys_close(int fd);

ssize_t
read(int fd, void *buf, size_t buflen);

