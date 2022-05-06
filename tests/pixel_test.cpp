#include <iostream>
#include "Pixel.h"

int main() {

    Pixel p1;
    Pixel p2(3.0);
    Pixel p3(1.0, 123.0, 1.2);
    p1.print();
    p2.print();
    p3.print();
    Pixel p4 = p2 + p3;
    p4.print();
    Pixel p5 = p1 + p4;
    p5.print();
    p1 += p2;
    p1.print();
    std::cout << "Test run successfully\n";
}