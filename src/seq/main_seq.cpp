#include <iostream>
#include "../common/utils.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <N>" << std::endl;
        return 1;
    }

    int N = std::stoi(argv[1]);
    float a = 2.5f;

    // Alocação alinhada (importante para comparação justa)
    float* x = allocate_aligned<float>(N);
    float* y = allocate_aligned<float>(N);

    init_vectors(x, y, N);

    Timer timer;
    timer.reset();

    // ============================================
    // V1: Kernel Sequencial
    // ============================================
    for (int i = 0; i < N; i++) {
        y[i] = a * x[i] + y[i];
    }

    double time = timer.elapsed();

    // Saída CSV: Versão, N, Threads, Tempo
    std::cout << "SEQ," << N << ",1," << std::fixed << std::setprecision(6) << time << std::endl;

    free_aligned(x);
    free_aligned(y);

    return 0;
}