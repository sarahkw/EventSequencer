#ifndef PRINTTO_QSTRING_H
#define PRINTTO_QSTRING_H

#include <QString>

inline void PrintTo(const QString &qString, ::std::ostream *os)
{
    *os << qUtf8Printable(qString);
}

#endif // PRINTTO_QSTRING_H
