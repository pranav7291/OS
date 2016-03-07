#include <kern/fcntl.h>

struct filedesc {
	struct vnode *fd_vnode;
	int fd_refcount;
	int fd_count;
	struct lock *fd_lock;
	char *name;
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
int sys_open(char *filename, int flags, int32_t *retval);

int sys_close(int fd, ssize_t *retval);

ssize_t read(int fd, void *buf, size_t buflen);

int sys_write(int fd, const void *buf, size_t size, ssize_t *retval);

