#ifndef POINT_H
#define POINT_H

#include <glm/glm.hpp>

template<class T>
class Point {
private: 
    T position;
    unsigned int index;
public:
    Point(T vec, unsigned int index);
    T getPosition() const;
    unsigned int getIndex() const;
};

#endif // POINT_H