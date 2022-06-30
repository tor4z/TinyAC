#ifndef PACK_COMMON_H_
#define PACK_COMMON_H_

#include <stdint.h>

#define MAX_PATH 512
#define BUFF_SIZE 1024


struct file_info
{
    uint32_t file_type;
    int64_t file_size;
    uint32_t file_mode;
    char path[MAX_PATH];
    unsigned char finish_flag;
};


int read_file_info(int infd, struct file_info *file_info);
int write_file_info(int outfd, struct file_info *file_info);
int big_endian();
int write_8bytes_number(int outfd, int64_t number);
int read_8bytes_number(int infd, int64_t *buffer);
int write_4bytes_number(int outfd, uint32_t number);
int read_4bytes_number(int infd, uint32_t *buffer);
int read_n_byte_string(int infd, char *str, int64_t n);
int write_n_byte_string(int outfd, char *str, int64_t n);
#endif // PACK_COMMON_H_
