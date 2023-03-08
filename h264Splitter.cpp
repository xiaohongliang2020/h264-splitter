#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#ifdef STARTER264NAL4
int STARTER264NAL = 4;
unsigned char starter[] = {0, 0, 0, 1};
#else
int STARTER264NAL = 3;
unsigned char starter[] = {0, 0, 1};
#endif

#define MB (1*1024*1024)
#define NALU_MAX_SIZE (16*MB)

static unsigned char *read_buffer = new unsigned char[NALU_MAX_SIZE];
static int read_buffer_len = 0;
static int read_buffer_ptr = 0;

void printUsage(const char *progname) {
    printf("%s inputfile\n", progname);
}

bool getOneNalu(int fd, unsigned char *buffer, int &length) {
    read_buffer_len = read(fd, read_buffer, NALU_MAX_SIZE);
    if (read_buffer_len <= 0) {
        return false;
    }
    int first_idx = -1;
    int second_idx = -1;
    for (int i = 0; i < read_buffer_len - STARTER264NAL; i++) {
        if (memcmp(read_buffer+i, starter, STARTER264NAL) == 0) {
            if (first_idx == -1) {
                first_idx = i;
            } else if (second_idx == -1) {
                second_idx = i;
            } else {
                break;
            }
        }
    }
    // last nalu
    if (second_idx == -1) {
        length = read_buffer_len;
        memcpy(buffer, read_buffer+first_idx, length);
    } else {
        length = second_idx - first_idx;
        memcpy(buffer, read_buffer+first_idx, length);
    }
    // go back
    int offset = (second_idx == -1) ? 0 : read_buffer_len - second_idx;
    lseek64(fd, -offset, SEEK_CUR);
    return true;
}

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }
    
    const char *inputfile = argv[1];

    int inFd = open(inputfile, O_RDONLY);
    if (inFd < 0) {
        printf("open input file failed\n");
        exit(EXIT_FAILURE);
    }
    unsigned char *nalu = new unsigned char[NALU_MAX_SIZE];
    int length;
    int idx = 1;
    char output_filename[256];
    while (getOneNalu(inFd, nalu, length)) {
        sprintf(output_filename, "%s.%04d", inputfile, idx);

        int outFd = open(output_filename, O_WRONLY | O_CREAT| O_TRUNC, 0644);
        if (outFd < 0) {
            printf("create file %s failed\n", output_filename);
            break;
        }

        int wt = write(outFd, nalu, length);

        if (wt != length) {
            printf("warn: write to file %s loss %d byte(s)\n", output_filename, (length - wt));
        }

        close(outFd);

        idx++;
    }
    
    delete[] nalu;
    delete[] read_buffer;
    close(inFd);

    return EXIT_SUCCESS;
}
