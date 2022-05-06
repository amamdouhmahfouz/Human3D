#include "Pixel.h"

Pixel::Pixel() {
    r = 0.0;
    g = 0.0;
    b = 0.0;
}

Pixel::Pixel(float c) {
    r = c; 
    g = c;
    b = c;
}

Pixel::Pixel(float r, float g, float b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

Pixel::~Pixel() {

}

Pixel Pixel::operator+(const Pixel& rhs) {
    float _r = this->r + rhs.r;
    float _g = this->g + rhs.g;
    float _b = this->b + rhs.b;

    return Pixel(_r, _g, _b);
}

Pixel& Pixel::operator+=(const Pixel& rhs) {
    this->r = this->r + rhs.r;
    this->g = this->g + rhs.g;
    this->b = this->b + rhs.b;
    return *this;
}

void Pixel::print() const {
    std::cout << "rgb(" << r << ", " << g << ", " << b << ")\n";
}