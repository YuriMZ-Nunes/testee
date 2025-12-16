#include <iostream>
#include <omp.h>
#include "../common/utils.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <N> <THREADS>" << std::endl;
        return 1;
    }

    int N = std::stoi(argv[1]);
    int num_threads = std::stoi(argv[2]);
    float a = 2.5f;

    omp_set_num_threads(num_threads);

    float* x = allocate_aligned<float>(N);
    float* y_v2 = allocate_aligned<float>(N); // Para teste SIMD
    float* y_v3 = allocate_aligned<float>(N); // Para teste Parallel SIMD

    // Inicializa todos iguais
    init_vectors(x, y_v2, N);
    init_vectors(x, y_v3, N);

    Timer timer;

    // ============================================
    // V2: OMP SIMD (Apenas vetorização, 1 thread)
    // ============================================
    // Nota: Rodamos isso apenas se threads == 1 no script,
    // ou rodamos sempre mas ignoramos no plot multi-thread.
    // Vamos medir aqui independente do número de threads setado (SIMD é intra-core).

    timer.reset();
    #pragma omp simd
    for (int i = 0; i < N; i++) {
        y_v2[i] = a * x[i] + y_v2[i];
    }
    double time_v2 = timer.elapsed();

    // ============================================
    // V3: OMP Parallel For SIMD (Multi-thread + Vetorização)
    // ============================================
    timer.reset();
    #pragma omp parallel for simd schedule(static)
    for (int i = 0; i < N; i++) {
        y_v3[i] = a * x[i] + y_v3[i];
    }
    double time_v3 = timer.elapsed();

    // Saída CSV: Versão, N, Threads, Tempo
    // V2 conta como 1 thread para fins de comparação lógica
    std::cout << "OMP_SIMD," << N << ",1," << time_v2 << std::endl;
    std::cout << "OMP_PAR_SIMD," << N << "," << num_threads << "," << time_v3 << std::endl;

    // Validação rápida (opcional, bom para debug)
    // check_result(y_v2, y_v3, N);

    free_aligned(x);
    free_aligned(y_v2);
    free_aligned(y_v3);

    return 0;
}