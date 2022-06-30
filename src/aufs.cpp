#include "tac/aufs.hpp"
#include "tac/log.hpp"
#include "tac/utils.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <linux/limits.h>
#include <sys/mount.h>


namespace tac
{

const char *AUFS::readOnlyName = "readOnly";
const char *AUFS::readWriteName = "readWrite";
const char *AUFS::mountintPointName = "mnt";


int AUFS::makeSpace(const char *containerPath)
{
    ASSERT0(Path::exists(containerPath))
        << "Container path not exists";

    char readOnlyPath[PATH_MAX];
    snprintf(
        readOnlyPath, sizeof(readOnlyPath),
        "%s/%s", containerPath, readOnlyName
    );
    char readWritePath[PATH_MAX];
    snprintf(
        readWritePath, sizeof(readOnlyPath),
        "%s/%s", containerPath, readWriteName
    );
    char mountingPointPath[PATH_MAX];
    snprintf(
        mountingPointPath, sizeof(readOnlyPath),
        "%s/%s", containerPath, mountintPointName
    );

    createReadOnly(readOnlyPath);
    createReadWrite(readWritePath);
    createMountingPoint(mountingPointPath);
    mountAUFS(readOnlyPath, readWritePath, mountingPointPath);
    return 0;
}


int AUFS::createReadOnly(const char *readOnlyPath)
{
    return Path::safe_mkdir(readOnlyPath, 0555);
}


int AUFS::createReadWrite(const char *readWriteName)
{
    return Path::safe_mkdir(readWriteName, 0777);
}


int AUFS::createMountingPoint(const char *mountintPointPath)
{
    return Path::safe_mkdir(readWriteName, 0777);
}


int AUFS::mountVolume(
    const char *volumePathPair,
    const char *mountingPointPath
)
{
    const char *splitPosition = strchr(volumePathPair, ':');
    int mntLen = strlen(mountingPointPath);
    char parentPath[PATH_MAX];
    char childPath[PATH_MAX];

    memcpy(childPath, mountingPointPath, mntLen);
    childPath[mntLen] = '/';
    int parentStrLen = splitPosition - volumePathPair;
    int childStrLen = strlen(volumePathPair) - parentStrLen - 1;

    memcpy(parentPath, volumePathPair, parentStrLen);
    memcpy(childPath + mntLen + 1 , splitPosition + 1, childStrLen);
    parentPath[parentStrLen] = '\0';
    childPath[childStrLen + mntLen + 1] = '\0';

    Path::safe_mkdir(parentPath, 077);
    Path::safe_mkdir(childPath, 077);

    char dirs[PATH_MAX + 5];
    snprintf(
        dirs, sizeof(dirs),
        "dirs=%s", parentPath
    );

    if (mountAUFS_(dirs, childPath) == -1)
    {
        PERROR() << "mount volume";
        return -1;
    }

    return 0;
}


int AUFS::mountAUFS(
    const char *ROPath, const char *RWPath,
    const char *mountingPointPath
)
{
    char dirs[PATH_MAX * 2 + 6];
    snprintf(
        dirs, sizeof(dirs),
        "%s:%s", RWPath, ROPath
    );

    if(mountAUFS_(dirs, mountingPointPath) == -1)
    {
        PERROR() << "mount AUFS error";
        return -1;
    }

    return 0;
}


int AUFS::mountAUFS_(
    const char *dirs,
    const char *mountingPointPath
)
{
    char cmd[256];
    snprintf(
        cmd, sizeof(cmd),
        "mount -t aufs -o dirs=%s none %s",
        dirs, mountingPointPath
    );
    return system(cmd);
}

int umountAUFS(const char *mountingPointPath)
{
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "umount %s", mountingPointPath);
    if(system(cmd) == -1)
    {
        PERROR() << "umount AUFS error";
        return -1;
    }
    return 0;
}


int AUFS::umountVolume(
    const char *volumePathPair,
    const char *mountingPointPath
)
{
    const char *splitPosition = strchr(volumePathPair, ':');
    int mntLen = strlen(mountingPointPath);
    char childPath[PATH_MAX];

    memcpy(childPath, mountingPointPath, mntLen);
    childPath[mntLen] = '/';
    int parentStrLen = splitPosition - volumePathPair;
    int childStrLen = strlen(volumePathPair) - parentStrLen - 1;

    memcpy(childPath + mntLen + 1 , splitPosition + 1, childStrLen);
    childPath[childStrLen + mntLen + 1] = '\0';

    if (umount(childPath) == -1)
    {
        PERROR() << "mount volume";
        return -1;
    }

    return 0;
}


}
