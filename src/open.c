#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    
    char pathname[] = "/tmp/test.txt";
    int fd = open(pathname, O_RDWR | O_CREAT, 0777);
    execlp("ls", "ls", "-l", "/tmp/", NULL);
    return 0;
}

