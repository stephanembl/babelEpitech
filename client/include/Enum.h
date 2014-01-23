#ifndef ENUM_H
#define ENUM_H

enum eStatus
{
    AVAILABLE = 0,
    BUSY,
    AWAY,
    DISCONNECTED
};

enum eIdentity
{
    ME = 0,
    NOT_ME
};

enum eStatusCall
{
    NONE,
    ISWAITING,
    ISCALLING,
    ISRINGING
};

#endif // ENUM_H
