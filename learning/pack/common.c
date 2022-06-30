#include "common.h"
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stddef.h>
#include <unistd.h>


static int big_endian_ = -1;


int write_file_info(int outfd, struct file_info *file_info)
{
    if(write_4bytes_number(outfd, file_info->file_type) <= 0)
        return -1;
    if (write_8bytes_number(outfd, file_info->file_size) <= 0)
        return -1;
    if (write_4bytes_number(outfd, file_info->file_mode) <= 0)
        return -1;
    if (write_n_byte_string(outfd, file_info->path, MAX_PATH) <= 0)
        return -1;
    if (write(outfd, &(file_info->finish_flag), sizeof(unsigned char)) <= 0)
        return -1;
    return 0;
}


int read_file_info(int infd, struct file_info *file_info)
{
    if (read_4bytes_number(infd, &(file_info->file_type)) <= 0)
        return -1;
    if (read_8bytes_number(infd, &(file_info->file_size)) <= 0)
        return -1;
    if (read_4bytes_number(infd, &(file_info->file_mode)) <= 0)
        return -1;
    if (read_n_byte_string(infd, file_info->path, MAX_PATH) <= 0)
        return -1;
    if (read(infd, &(file_info->finish_flag), sizeof(unsigned char)) <= 0)
        return -1;
    return 0;
}


int big_endian()
{
    if(big_endian_ == -1)
    {
        uint16_t n = 1;
        if ((n & 0x0001) == 1)
            big_endian_ = 0;
        else
            big_endian_ = 1;
    }
    return big_endian_;
}


int read_8bytes_number(int infd, int64_t *buffer)
{
    int res;
    if ((res = read(infd, buffer, 8)) <= 0)
        return res;

    if(!big_endian())
    {
        unsigned char tmp;
        for (int i = 0; i < 4; i++)
        {
            tmp = ((char*)buffer)[i];
            ((char*)buffer)[i] = ((char*)buffer)[7 - i];
            ((char*)buffer)[7 - i] = tmp;
        }
    }
    // write 8 bytes
    return 8;
}


int write_8bytes_number(int outfd, int64_t number)
{
    unsigned char *buff = (unsigned char*)&number;
    if (!big_endian())
    {
        // reverse
        unsigned char tmp;
        for (int i = 0; i < 4; i++)
        {
            tmp = buff[i];
            buff[i] = buff[7 - i];
            buff[7 - i] = tmp;
        }
    }
    return write(outfd, buff, 8);
}


int read_4bytes_number(int infd, uint32_t *buffer)
{
    int res;
    if ((res = read(infd, buffer, 4)) <= 0)
        return res;

    if(!big_endian())
    {
        unsigned char tmp;
        for (int i = 0; i < 2; i++)
        {
            tmp = ((char*)buffer)[i];
            ((char*)buffer)[i] = ((char*)buffer)[3 - i];
            ((char*)buffer)[3 - i] = tmp;
        }
    }
    // write 8 bytes
    return 4;
}


int write_4bytes_number(int outfd, uint32_t number)
{
    unsigned char *buff = (unsigned char*)&number;
    if (!big_endian())
    {
        // reverse
        unsigned char tmp;
        for (int i = 0; i < 2; i++)
        {
            tmp = buff[i];
            buff[i] = buff[3 - i];
            buff[3 - i] = tmp;
        }
    }
    return write(outfd, buff, 4);
}


int read_n_byte_string(int infd, char *str, int64_t n)
{
    ssize_t nread;
    int64_t data_to_read = n;
    char *read_ptr = str;

    for(;;)
    {
        nread = read(infd, read_ptr, n);
        if (nread < 0) // read fail
            return -1;
        n -= nread;
        read_ptr += nread;

        if(errno == EINTR)
            return -1;
        if (n == 0)  // read finished
            return data_to_read;
    }
    
    // never reach here
    return data_to_read;
}


int write_n_byte_string(int outfd, char *str, int64_t n)
{
    ssize_t nwrite;
    int64_t data_to_write = n;
    char *write_ptr = str;

    for (;;)
    {
        nwrite = write(outfd, write_ptr, n);
        if (nwrite <= 0) // write fail
            return -1;
        n -= nwrite;
        write_ptr += nwrite;

        if (errno == EINTR) // write error
            return -1;
        if (n == 0) // write finished
            return data_to_write;
    }

    // never reach here
    return data_to_write;
}