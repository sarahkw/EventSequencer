#ifndef PRINTTO_CHANNELINDEX_H
#define PRINTTO_CHANNELINDEX_H

#include "channelindex.h"

inline void PrintTo(const ChannelIndex &val, ::std::ostream *os)
{
    *os << val.toDebugString().toStdString();
}

#endif // PRINTTO_CHANNELINDEX_H
