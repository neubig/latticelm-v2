
#include <latticlem/latticlem-train.h>
#include <cnn/init.h>

using namespace latticelm;

int main(int argc, char** argv) {
    cnn::Initialize(argc, argv);
    LatticeLMTrain train;
    return train.main(argc, argv);
}
