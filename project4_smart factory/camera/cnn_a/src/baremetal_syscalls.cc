#include <sys/stat.h>

extern "C" {

int _open(const char* path, int flags, int mode)
{
	(void)path;
	(void)flags;
	(void)mode;
	return -1;
}

int _fstat(int fd, struct stat* st)
{
	(void)fd;
	if (st != 0) {
		st->st_mode = S_IFCHR;
	}
	return 0;
}

int _isatty(int fd)
{
	(void)fd;
	return 1;
}

}
