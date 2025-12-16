#include <iostream>
#include <omp.h>
#include <vector>
#include "../common/utils.h"

// Função kernel simples para gastar tempo
// Operação: y = x * scale + offset
void kernel_work(int n, float* x, float* y, float scale, float offset) {
    // Nota: O schedule e chunk não são o foco aqui, deixamos o default
    #pragma omp for 
    for (int i = 0; i < n; i++) {
        y[i] = x[i] * scale + offset;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <N> <THREADS>" << std::endl;
        return 1;
    }

    int N = std::stoi(argv[1]);
    int num_threads = std::stoi(argv[2]);

    omp_set_num_threads(num_threads);

    // Alocação de vetores (usando utils.h)
    float* A = allocate_aligned<float>(N);
    float* B = allocate_aligned<float>(N);
    float* C = allocate_aligned<float>(N); // Vetor auxiliar

    init_vectors(A, B, N);

    Timer timer;
    double time_naive = 0.0;
    double time_smart = 0.0;

    // ============================================
    // V1: Ingênua (Dois parallel for consecutivos)
    // ============================================
    // Aqui ocorre o Fork/Join DUAS vezes.
    
    timer.reset();
    
    // Região Paralela 1
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        B[i] = A[i] * 2.0f;
    }

    // Região Paralela 2 (Depende de B, mas cria novas threads)
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        C[i] = B[i] + 10.0f;
    }
    
    time_naive = timer.elapsed();

    // ============================================
    // V2: Arrumada (Uma região parallel com dois for)
    // ============================================
    // Aqui ocorre o Fork/Join UMA vez.
    
    // Resetar dados para garantir justiça (opcional, mas bom pra evitar cache quente viciado)
    // init_vectors(A, B, N); 

    timer.reset();

    #pragma omp parallel
    {
        // Work-sharing 1
        // (threads já estão vivas aqui)
        #pragma omp for
        for (int i = 0; i < N; i++) {
            B[i] = A[i] * 2.0f;
        }
        // Existe uma barreira implícita aqui, necessária pois o próximo loop lê B.
        // Mas as threads NÃO são destruídas.

        // Work-sharing 2
        #pragma omp for
        for (int i = 0; i < N; i++) {
            C[i] = B[i] + 10.0f;
        }
    } // Fim da região paralela (Join único)

    time_smart = timer.elapsed();

    // Saída CSV
    std::cout << "OMP_NAIVE," << N << "," << num_threads << "," << time_naive << std::endl;
    std::cout << "OMP_SMART," << N << "," << num_threads << "," << time_smart << std::endl;

    free_aligned(A);
    free_aligned(B);
    free_aligned(C);

    return 0;
}