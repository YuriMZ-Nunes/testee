#!/bin/bash

# Arquivo de saída
OUTPUT="results/dados.csv"

# Garante que a pasta existe
mkdir -p results

# Cabeçalho do CSV
echo "Version,N,Threads,Time" > $OUTPUT

echo "========================================"
echo "Iniciando Bateria de Testes"
echo "Saída: $OUTPUT"
echo "========================================"

# Definição dos parâmetros
SIZES=(100000 500000 1000000)
THREADS=(1 2 4 8 16)
REPEATS=5

# 1. Executar Sequencial (Tarefa C - V1)
# O sequencial independe de threads, rodamos apenas variando N
echo "Rodando Sequencial..."
for N in "${SIZES[@]}"; do
    for ((i=1; i<=REPEATS; i++)); do
        ./bin/task_c_seq $N >> $OUTPUT
    done
done

# 2. Executar OMP Tarefa C (Vetorização)
echo "Rodando Tarefa C (SIMD/Parallel)..."
for N in "${SIZES[@]}"; do
    for T in "${THREADS[@]}"; do
        for ((i=1; i<=REPEATS; i++)); do
            # O binário roda V2 (SIMD puro) e V3 (Paralelo) internamente
            ./bin/task_c_omp $N $T >> $OUTPUT
        done
    done
done

# 3. Executar OMP Tarefa D (Overhead)
echo "Rodando Tarefa D (Region Overhead)..."
for N in "${SIZES[@]}"; do
    for T in "${THREADS[@]}"; do
        for ((i=1; i<=REPEATS; i++)); do
            ./bin/task_d_omp $N $T >> $OUTPUT
        done
    done
done

echo "========================================"
echo "Testes finalizados."
echo "========================================"