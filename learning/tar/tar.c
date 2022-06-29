#include <stdio.h>
#include <stdlib.h>
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


int create(const char *filename, char *dirs[], int numDirs)
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

    while (numDirs)
    {
        disk = archive_read_disk_new();
#ifndef NO_LOOKUP
        archive_read_disk_set_standard_lookup(disk);
#endif

        res = archive_read_disk_open(disk, *dirs);
        if (res != ARCHIVE_OK)
        {
            fprintf(
                stderr, "read disk fail: %s\n",
                archive_error_string(disk)
            );
            ret = -1;
            break;
        }

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

        archive_read_close(disk);
        archive_read_free(disk);
        --numDirs;
        if (ret != 0 || numDirs == 0)
            break;
        ++dirs;
    }

    archive_write_close(a);
    archive_write_free(a);
    return ret;
}


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("argument error\n");
        exit(1);
    }

    const char *filename = argv[1];
    argv += 2;
    create(filename, argv, argc - 2);

    return 0;
}
