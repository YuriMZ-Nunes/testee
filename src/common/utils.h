#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <iomanip>

// Alinhamento para AVX (32 bytes) ou AVX-512 (64 bytes).
// 64 é seguro para ambos e cabe numa linha de cache.
constexpr std::size_t ALIGNMENT = 64;

// Alocador de memória alinhada
template <typename T>
T* allocate_aligned(size_t size) {
    void* ptr = nullptr;
    // posix_memalign é robusto no Linux
    if (posix_memalign(&ptr, ALIGNMENT, size * sizeof(T)) != 0) {
        throw std::bad_alloc();
    }
    return static_cast<T*>(ptr);
}

// Liberador de memória
template <typename T>
void free_aligned(T* ptr) {
    free(ptr);
}

// Classe simples para medir tempo (funciona sem OpenMP linkado)
class Timer {
    std::chrono::high_resolution_clock::time_point start;
public:
    void reset() { start = std::chrono::high_resolution_clock::now(); }
    double elapsed() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        return diff.count();
    }
};

// Inicializa vetores
void init_vectors(float* x, float* y, int n) {
    for (int i = 0; i < n; i++) {
        x[i] = 1.0f;       // Exemplo simples
        y[i] = 0.5f;
    }
}

// Verifica erro relativo entre dois vetores (Gold Standard vs Teste)
bool check_result(const float* gold, const float* test, int n) {
    for (int i = 0; i < n; i++) {
        if (std::abs(gold[i] - test[i]) > 1e-5) {
            std::cerr << "Erro na posicao " << i << ": Esperado " << gold[i] 
                      << ", Obtido " << test[i] << std::endl;
            return false;
        }
    }
    return true;
}

#endif