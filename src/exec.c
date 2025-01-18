#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	execl("/bin/sh", "sh", "-c", "ls /tmp > /tmp/output.txt", (char *)NULL);
}
