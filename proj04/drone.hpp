// Project Identifier: 1761414855B69983BD8035097EFBD312EB0527F0
#include <string>
#include <limits> //this is for infinity

using namespace std;

double infinity = numeric_limits<double>::infinity();

struct Vertex{
    double dedge = infinity;
    int pidx = -1; // pre idx
    uint32_t deg = 0;
    bool visited = false;
};

enum class Campus {border, normal, medical};
class Drop{
public:
    int x, y;
    Campus campus = Campus::normal;
    Drop(int xin, int yin) : x(xin), y(yin) {
        if (((xin == 0 && yin <= 0) || (xin <= 0 && yin == 0)))
            campus = Campus::border;
        else if (xin < 0 && yin < 0)
            campus = Campus::medical;
    };

};

//CHRISTOFIDE HELPER FUNCTIONS

    //EULER CIRCUIT
