#ifndef COLLIDES_H
#define COLLIDES_H

struct Collides {
    Collides() = delete;
    static bool startAndLength(int start1, int length1, int start2, int length2);

    template <typename T>
    static bool startAndLengthMembers(const T& a, const T& b)
    {
        return Collides::startAndLength(a.start, a.length, b.start, b.length);
    }
};

#endif // COLLIDES_H
