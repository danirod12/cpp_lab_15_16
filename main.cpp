#include <iostream>
#include <fstream>
#include <valarray>
#include <iomanip>
#include "Matrix.hpp"

long long callOperation(long size, int blocks);

int main() {
    std::ofstream out("./../data.csv");
    out << std::setprecision(50);
    out << ";";
    for (int blocks = 0; blocks <= 24; blocks += 2) {
        out << (blocks == 0 ? 1 : blocks) << ";";
    }
    out << "\n";

    for (int size = 1; size <= 10; ++size) {
        long ySize = pow(2.5, size);
        out << ySize * ySize << ";";
        for (int blocks = 0; blocks <= 24; blocks += 2) {
            out << callOperation(ySize, blocks) << ";";
        }
        out << "\n";
        std::cout << "Completed for " << size << "/10\n";
    }
    return 0;
}

long long callOperation(long size, int blocks) {
    Matrix<int> matrix(size, size);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix.setValue(i, j, rand() % 1000);
        }
    }

    auto startAsync = std::chrono::high_resolution_clock::now();
    matrix.sumWithAsync(matrix, blocks).get();
    auto endAsync = std::chrono::high_resolution_clock::now();
    return (endAsync - startAsync).count() / 1000L;
}
