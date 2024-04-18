#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <getopt.h>

#define I2C_DEVICE "/dev/pcf8563dev" // I2C 设备节点路径

void usage(char *progname) {
    printf("Usage: %s [-r <address> <register>] [-w <address> <register> <value>]\n", progname);
}

int main(int argc, char *argv[]) {
    int file;
    int opt;
    __u8 reg, val;
    __u8 buf[2];
    int cmd;

    if (argc < 3) {
        usage(argv[0]);
        return 1;
    }

    if ((file = open(I2C_DEVICE, O_RDWR)) < 0) {
        perror("Failed to open the I2C device");
        return 1;
    }
    printf("开始解析参数:\n");

    while ((opt = getopt(argc, argv, "r:w:d:")) != -1) {
        printf("开始解析参数: optind = %d\n", optind);
        switch (opt) {
            case 'r':
                cmd = opt;
                reg = (__u8)atoi(optarg);
                
                break;
            case 'w':
                cmd = opt;
                buf[0] = (__u8)atoi(optarg);
                break;
            case 'd':
                buf[1] = (__u8)atoi(optarg);
            default:
                usage(argv[0]);
                close(file);
                return 1;
        }
    }

    if (cmd == 'w') {
        if (write(file, buf, 2) != 2) {
            perror("Failed to write to the I2C device");
            close(file);
            return 1;
        }
        printf("Write value 0x%x to register 0x%x\n", val, reg);
    }else if (cmd == 'r') {
        if (write(file, &reg, 1) != 1) {
            perror("Failed to set reg");
            close(file);
            return 1;
        }
        if (read(file, buf, 1) != 1) {
            perror("Failed to read from the I2C device");
            close(file);
            return 1;
        }
        printf("Register 0x%x value: 0x%02x\n", reg, buf[0]);
    }

    close(file);
    return 0;
}
