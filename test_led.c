#include <stdio.h>
#include <fcntl.h>      // open()
#include <unistd.h>     // write(), close()
#include <string.h>

int main() {
    int fd = open("/dev/myled", O_WRONLY);  // Open device like a file
    if (fd < 0) {
        perror("Failed to open /dev/myled");
        return 1;
    }

    printf("LED ON\n");
    write(fd, "1", 1);
    sleep(2);

    printf("LED OFF\n");
    write(fd, "0", 1);

    close(fd);
    return 0;
}
