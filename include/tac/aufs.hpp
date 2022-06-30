#pragma once

#include <string>


namespace tac
{

class AUFS
{
public:
    static int makeSpace(const char *containerPath);
    static int createReadOnly(const char *containerPath);
    static int createReadWrite(const char *containerPath);
    static int createMountingPoint(const char *mountintPointPath);
    static int mountVolume(
        const char* volumePathPair,
        const char *mountingPointPath
    );
    static int mountAUFS(
        const char *ROPath,
        const char *RWPath,
        const char *mountingPointPath
    );
    static int umountAUFS(const char *mountingPointPath);
    static int umountVolume(
        const char *volumePathPair,
        const char *mountingPointPath
    );

private:
    static const char *readOnlyName;
    static const char *readWriteName;
    static const char *mountintPointName;
    static int mountAUFS_(
        const char *dirs,
        const char *mountingPointPath
    );
};

}
