# Informações de Reprodutibilidade

Este documento descreve o ambiente de hardware e software utilizado para gerar os resultados apresentados no arquivo `RESULTADOS.md`, garantindo a reprodutibilidade dos experimentos.

## 1. Hardware

* **Processador (CPU):** AMD Ryzen 7 7735HS with Radeon Graphics
    * **Arquitetura:** x86_64
    * **Núcleos Físicos:** 8
    * **Threads por Núcleo:** 2
    * **Total de CPUs Lógicas:** 16
* **Memória RAM:** 19Gi

## 2. Software

* **Sistema Operacional:** Fedora Linux
* **Kernel:** 6.17.11-300.fc43.x86_64
* **Compilador:** GCC (G++)
    * **Versão:** g++ (GCC) 15.2.1 20251211 (Red Hat 15.2.1-5)

## 3. Configuração de Compilação (Build)

O projeto utiliza um `Makefile` para automação. As flags utilizadas foram escolhidas para maximizar o desempenho em hardware moderno e habilitar o suporte a OpenMP e Vetorização.

* **Flags de Otimização:** `-O3` (Nível máximo de otimização estável)
* **Flags de Arquitetura:** `-march=native` (Habilita instruções específicas da CPU local, como AVX/AVX2, essenciais para a Tarefa C - SIMD)
* **Flags OpenMP:** `-fopenmp`
* **Padrão C++:** `-std=c++17`

**Comando completo de compilação:**
```bash
g++ -O3 -march=native -Wall -std=c++17 -fopenmp -o bin/nome_executavel src/arquivo.cpp
```

## 4. Execução e Afinidade

### Afinidade de Threads
Para os testes realizados, não foi forçada uma afinidade explícita via variáveis de ambiente (como `OMP_PROC_BIND` ou `OMP_PLACES`).

* O escalonamento das threads ficou a cargo do Sistema Operacional (CFS - Completely Fair Scheduler do Linux).
* Isso reflete um cenário de uso geral, embora possa introduzir variância (tratada no projeto através da execução de 5 repetições e cálculo do desvio padrão).

### Semente do Gerador (Seed)

* **Determinismo:** Os algoritmos implementados (SAXPY e Kernels de Teste) utilizam inicialização determinística dos vetores (valores fixos ou calculados via índice `i`).
* **RNG:** Não foram utilizados geradores de números pseudoaleatórios (como `rand()` ou `std::mt19937`), portanto, não há semente (`seed`) variável. Isso garante que os cálculos sejam bit-a-bit idênticos em todas as execuções, isolando a variação de tempo causada apenas pelo paralelismo.

