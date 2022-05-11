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
