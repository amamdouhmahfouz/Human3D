#include <iostream>

class Pixel {
public: 
    float r, g, b;
    Pixel();
    Pixel(float c);
    Pixel(float r, float g, float b);
    ~Pixel();
    void print() const;

    Pixel operator+(const Pixel& rhs);
    Pixel& operator+=(const Pixel& rhs);
};