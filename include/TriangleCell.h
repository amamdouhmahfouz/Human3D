#ifndef TRIANGLECELL_H
#define TRIANGLECELL_H

class TriangleCell {
public:
    unsigned int indexVertex1;
    unsigned int indexVertex2;
    unsigned int indexVertex3;

    unsigned int indexNormal;

    unsigned int triangleIndex; // face index

    TriangleCell(unsigned int v1, unsigned int v2, unsigned int v3,
                 unsigned int n1, unsigned int index) : indexVertex1(v1), indexVertex2(v2), 
                    indexVertex3(v3), indexNormal(n1), triangleIndex(index) {}
};

#endif // TRIANGLECELL_H