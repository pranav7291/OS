#include <unistd.h>
#include <fcntl.h>

/**	added by sammokka
 O_RDONLY		Open for reading only.
 O_WRONLY		Open for writing only.
 O_RDWR		Open for reading and writing.
 */
int
sys_open(const char *filename, int flags);


int
sys_close(int fd);

ssize_t
read(int fd, void *buf, size_t buflen);
