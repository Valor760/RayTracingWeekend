#include "Program.h"


int main(int argc, char** argv) {
    const int width = 1600;
    const int height = 900;

    RTWeekend::Program program(width, height);
    program.Run();

    return 0;
}