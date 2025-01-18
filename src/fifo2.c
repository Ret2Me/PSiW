#include <fcntl.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main() {

	// tworzy nowe fifo w folderze /tmp/ z permisjami do odczytu i zapisu
	char path[] = "/tmp/put_fifo";
    if (mkfifo(path, 0666) == -1)
        if (errno == EEXIST)
            printf("FIFO already exists: %s\n", path);
	
	if (open(path, O_WRONLY) == -1) {
		perror("open");
	}
}
