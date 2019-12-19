#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include<fcntl.h>
#include <linux/watchdog.h>
#include <stdlib.h>

int fd;

void catch_int(int signum)
{
    signal(SIGINT, catch_int);

    printf("In signal handler\n");
    if(0 != close(fd))
        printf("Close failed in signal handler\n");
    else
        printf("Close succeeded in signal handler\n");
}

void usage(int argc, const char *argv[])
{
    printf("usage: %s /dev/watchdogX [second]\n", argv[0]);
}

int main(int argc, const char *argv[]) 
{
    int sleep_time;
    int data = 0;
    int ret_val;

    signal(SIGINT, catch_int);

    if (argc == 3) {
        sleep_time = atoi(argv[2]);
        if (sleep_time == 0) {
            usage(argc, argv);
            return -1;
        }
    }
    else if (argc == 2) {
        sleep_time = 0;
    } else {
        usage(argc, argv);
        return -1;
    }
    
    fd = open(argv[1], O_WRONLY);
    if (fd==-1) {
        perror("watchdog");
        return -1;
    }

    if (sleep_time == 0) {
        ret_val = ioctl (fd, WDIOC_GETTIMELEFT, &data);
        if (ret_val) {
            printf("Not support to read left time\n");
        }
        else {
            printf("System will reboot in %d second\n", data);
        }
    } else {
        data = sleep_time;
        ret_val = ioctl(fd, WDIOC_SETTIMEOUT, &data);
        if (ret_val) {
            printf("WDIOC_SETTIMEOUT failed\n");
        }
        else {
            printf("Set timeout: %d seconds\n", data);
        }

        ret_val = ioctl(fd, WDIOC_GETTIMEOUT, &data);
        if (ret_val) {
            printf("WDIOC_GETTIMEOUT failed\n");
        }
        else {
            printf("Get timeout: %d seconds\n", data);
        }

        if (1 != write(fd, "\0", 1))
            printf("Write failed\n");
        else
            printf("System will reboot in %d second\n", sleep_time);
    }
    close (fd);
    return 0;
}
