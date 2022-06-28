#include <stdio.h>
#include <bits/stdint-intn.h>
#include <stdlib.h>
#include <stdio.h>
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


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


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "param error\n");
        return -1;
    }
    extract(argv[1]);
    return 0;
}

