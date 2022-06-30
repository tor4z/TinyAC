#include "tac/tar.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


namespace tac
{

int create(const char *filename, const char *dir)
{
    struct archive *a;
    struct archive *disk;
    struct archive_entry *entry;
    ssize_t len;
    char buffer[16384];
    int fd, res;
    int ret = 0;

    a = archive_write_new();
    archive_write_add_filter_none(a);
    archive_write_set_format_ustar(a);
    res = archive_write_open_filename(a, filename);
    if (res != ARCHIVE_OK)
    {
        fprintf(
            stderr, "open write file fail: %s\n",
            archive_error_string(a)
        );
        return 1;
    }

    disk = archive_read_disk_new();
#ifndef NO_LOOKUP
    archive_read_disk_set_standard_lookup(disk);
#endif

    res = archive_read_disk_open(disk, dir);
    if (res != ARCHIVE_OK)
    {
        fprintf(
            stderr, "read disk fail: %s\n",
            archive_error_string(disk)
        );
        ret = -1;
    }
    else
    {
        for (;;)
        {
            entry = archive_entry_new();
            res = archive_read_next_header2(disk, entry);
            if (res == ARCHIVE_EOF)
            {
                ret = 0;
                break;
            }
            if (res != ARCHIVE_OK)
            {
                fprintf(
                    stderr, "read next header2 fail(%d): %s\n",
                    res, archive_error_string(disk)
                );
                ret = -1;
                break;
            }

            archive_read_disk_descend(disk);
            res = archive_write_header(a, entry);
            printf("process %s\n", archive_entry_pathname(entry));
            if (res < ARCHIVE_OK)
            {
                fprintf(
                    stderr, "write header fail (%d): %s\n",
                    res, archive_error_string(a)
                );
                if (res == ARCHIVE_FATAL)
                {
                    ret = -1;
                    break;
                }
            }
            if (res > ARCHIVE_FAILED)
            {
                fd = open(archive_entry_sourcepath(entry), O_RDONLY);
                len = read(fd, buffer, sizeof(buffer));
                while (len > 0)
                {
                    archive_write_data(a, buffer, len);
                    len = read(fd, buffer, sizeof(buffer));
                }
                close(fd);
            }
            archive_entry_free(entry);
        }
    }

    archive_read_close(disk);
    archive_read_free(disk);
    archive_write_close(a);
    archive_write_free(a);
    return ret;
}


int copy_data(struct archive *ar, struct archive *aw)
{
    int res;
    const void *buffer;
    size_t size;
    la_int64_t offset;

    for(;;)
    {
        res = archive_read_data_block(ar, &buffer, &size, &offset);
        if (res == ARCHIVE_EOF)
            break;
        if (res != ARCHIVE_OK)
        {
            fprintf(
                stderr,
                "read archive error: %s, %d\n", archive_error_string(ar), res
            );
            return -1;
        }

        res = archive_write_data_block(aw, buffer, size, offset);
        if(res != ARCHIVE_OK)
        {
            fprintf(stderr, "write data error\n");
            return -1;
        }
    }

    return 0;
}


int extract(const char *filename)
{
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int flags = ARCHIVE_EXTRACT_TIME;
    int res, ret = 0;

    a = archive_read_new();
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_read_support_format_tar(a);

    res = archive_read_open_filename(a, filename, 10240);
    if (res != ARCHIVE_OK)
    {
        fprintf(stderr, "open file failed.\n");
        archive_read_close(a);
        archive_read_free(a);
        archive_write_close(ext);
        archive_write_free(ext);
        return -1;
    }

    for (;;)
    {
        res = archive_read_next_header(a, &entry);
        if (res == ARCHIVE_EOF)
        {
            ret = 0;
            break;
        }
        if (res != ARCHIVE_OK)
        {
            fprintf(stderr, "read entry faild\n");
            ret = -1;
            break;
        }
        printf("%s\n", archive_entry_pathname(entry));

        res = archive_write_header(ext, entry);
        if (res != ARCHIVE_OK)
        {
            fprintf(stderr, "write header fail\n");
            ret = -1;
            break;
        }

        if (copy_data(a, ext) == -1)
        {
            ret = -1;
            break;
        }
        res = archive_write_finish_entry(ext);
        if (res != ARCHIVE_OK)
        {
            fprintf(stderr, "write finish entry fail\n");
            ret = -1;
            break;
        }
    }

    archive_read_close(a);
    archive_read_free(a);

    archive_write_close(ext);
    archive_write_free(ext);

    return ret;
}


int tar(const char *imagePath, const char *dir)
{
    return create(imagePath, dir);
}


int untar(const char *imageFilename)
{
    return 0;
}

}
