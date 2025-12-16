# Relatório de Resultados: Paralelismo com OpenMP

## 1. Tarefa C: Vetorização e Paralelismo (SAXPY)

### 1.1. Resultados (Para N = 1.000.000)
A tabela abaixo destaca o desempenho com o maior vetor testado (1 milhão de elementos).

| Versão | Threads | Tempo Médio (s) | Speedup vs SEQ | Observação |
|---|---|---|---|---|
| **SEQ** | 1 | 0.000304 | 1.00x | Base |
| **OMP_SIMD** | 1 | 0.000290 | 1.05x | Ganho marginal (SIMD) |
| **OMP_PAR_SIMD** | 1 | 0.000243 | 1.25x | Melhor que SIMD puro |
| **OMP_PAR_SIMD** | 2 | 0.000262 | 1.16x | Leve degradação |
| **OMP_PAR_SIMD** | 4 | **0.000239** | **1.27x** | **Melhor caso (Pico)** |
| **OMP_PAR_SIMD** | 8 | 0.000322 | 0.94x | Piora (Saturação) |
| **OMP_PAR_SIMD** | 16 | 0.004138 | 0.07x | Colapso (Oversubscription) |

### 1.2. Gráficos de Desempenho

O primeiro gráfico ilustra o ganho obtido apenas com a vetorização (SIMD) em uma única thread, comparado à versão sequencial base. Observa-se um ganho modesto, mas consistente, para todos os tamanhos de vetor.

![Ganho da Vetorização (SIMD) em Single Thread](results/grafico_tarefa_c_simd_impact.png.png)

O segundo gráfico mostra a escalabilidade da versão paralela vetorizada (`OMP_PAR_SIMD`). É evidente a rápida saturação do speedup, que não ultrapassa 1.4x mesmo com várias threads, e o colapso de desempenho ao utilizar 16 threads, ficando pior que a versão sequencial.

![Escalabilidade da Tarefa C (Parallel SIMD vs Sequencial)](results/grafico_tarefa_c_speedup.png.png)

### 1.3. Análise Crítica

1.  **Gargalo de Memória (Memory Wall):**
    * O algoritmo SAXPY ($Y = aX + Y$) é uma operação classificada como *Memory Bound*. Ele realiza pouquíssimos cálculos por byte transferido da memória RAM.
    * Como observado na tabela e no gráfico de escalabilidade, o ganho máximo foi pequeno (~1.27x com 4 threads). Isso ocorre porque a largura de banda da memória saturou. As CPUs ficaram ociosas esperando dados chegarem da RAM, impedindo escalabilidade linear.

2.  **Vetorização (SIMD):**
    * O gráfico de ganho SIMD mostra que a versão `OMP_SIMD` foi ligeiramente mais rápida que a sequencial para todos os N testados, com speedups entre 1.05x e 1.12x. Isso confirma que as instruções vetoriais (AVX) foram ativadas, mas o ganho total foi limitado pela velocidade de acesso à memória.

3.  **Oversubscription (16 Threads):**
    * Houve uma degradação severa com 16 threads, onde o tempo subiu drasticamente. Isso indica que o número de threads excedeu o número de núcleos físicos disponíveis na CPU. O Sistema Operacional gastou a maior parte do tempo realizando *Context Switching* (troca de contexto) entre as threads em vez de calcular.

---

## 2. Tarefa D: Organização de Região Paralela

### 2.1. Resultados (Para N = 500.000)
Comparação do *overhead* entre criar threads repetidamente (Naive) vs reutilizá-las (Smart).

| Versão | Threads | Tempo Médio (s) | Desvio Padrão (s) | Status |
|---|---|---|---|---|
| **OMP_NAIVE** | 4 | 0.000573 | 0.000062 | Lento |
| **OMP_SMART** | 4 | **0.000064** | 0.000004 | **Rápido** |
| **OMP_NAIVE** | 8 | 0.000547 | 0.000040 | Lento |
| **OMP_SMART** | 8 | 0.000064 | 0.000004 | **Rápido** |

### 2.2. Gráfico de Overhead
O gráfico abaixo evidencia visualmente a enorme diferença de tempo de execução entre a abordagem ingênua (azul) e a arrumada (vermelho) para N=500.000. A versão arrumada mantém um tempo de execução baixo e constante, enquanto a ingênua é significativamente mais lenta e apresenta maior variação (indicada pelas barras de erro) e um aumento de tempo com 16 threads.

![Comparação de Overhead na Tarefa D](results/grafico_tarefa_d_overhead.png.png)

### 2.3. Análise de Overhead

* **Impacto do Fork/Join:**
    * A versão `OMP_NAIVE` levou **~573µs** com 4 threads, enquanto a `OMP_SMART` levou apenas **~64µs**.
    * Isso representa um **Speedup de aproximadamente 9x** apenas reorganizando a região paralela.

* **Interpretação:**
    * A diferença de tempo é o custo puro do sistema operacional para criar (*fork*) e destruir (*join*) o time de threads repetidamente na versão ingênua.
    * Na versão `SMART`, as threads são criadas uma única vez e mantidas vivas, eliminando a latência de alocação de recursos do SO. Conforme mostrado no gráfico, a linha vermelha (Smart) permanece próxima de zero, demonstrando a eficácia dessa abordagem para cargas de trabalho leves.

---

## 3. Decisões de Projeto

### 3.1. Escolha de Schedule e Chunk
* **Política:** `schedule(static)` (Padrão/Default).
* **Justificativa:** Os problemas abordados possuem iterações com carga de trabalho constante e previsível. O uso de `schedule(dynamic)` introduziria um overhead desnecessário. O `static` favorece a localidade espacial de cache.

### 3.2. Critério: Atomic vs Critical
*Embora os dados tabulados foquem em C e D, o critério técnico adotado para sincronização (se necessário) foi:*
* **Uso de Atomic:** Preferível para atualizações simples em escalares (ex: `soma += valor`), pois usa instruções de hardware sem bloquear todo o pipeline.
* **Uso de Critical:** Evitado em loops frequentes, pois serializa a execução.

---
## Apêndice: Tabela Completa de Dados Brutos

| Versão | N | Threads | Tempo Médio (s) | Desvio Padrão (s) |
|---|---|---|---|---|
| OMP_NAIVE | 100000 | 1 | 0.000194 | 0.000039 |
| OMP_NAIVE | 100000 | 2 | 0.000186 | 0.000049 |
| OMP_NAIVE | 100000 | 4 | 0.000193 | 0.000027 |
| OMP_NAIVE | 100000 | 8 | 0.000249 | 0.000015 |
| OMP_NAIVE | 100000 | 16 | 0.003982 | 0.005645 |
| OMP_NAIVE | 500000 | 1 | 0.001019 | 0.000149 |
| OMP_NAIVE | 500000 | 2 | 0.000851 | 0.000082 |
| OMP_NAIVE | 500000 | 4 | 0.000573 | 0.000062 |
| OMP_NAIVE | 500000 | 8 | 0.000547 | 0.000040 |
| OMP_NAIVE | 500000 | 16 | 0.000837 | 0.000131 |
| OMP_NAIVE | 1000000 | 1 | 0.002195 | 0.000367 |
| OMP_NAIVE | 1000000 | 2 | 0.001462 | 0.000197 |
| OMP_NAIVE | 1000000 | 4 | 0.001159 | 0.000077 |
| OMP_NAIVE | 1000000 | 8 | 0.000981 | 0.000061 |
| OMP_NAIVE | 1000000 | 16 | 0.016397 | 0.004391 |
| OMP_PAR_SIMD | 100000 | 1 | 0.000017 | 0.000003 |
| OMP_PAR_SIMD | 100000 | 2 | 0.000076 | 0.000022 |
| OMP_PAR_SIMD | 100000 | 4 | 0.000111 | 0.000018 |
| OMP_PAR_SIMD | 100000 | 8 | 0.000174 | 0.000008 |
| OMP_PAR_SIMD | 100000 | 16 | 0.002035 | 0.003707 |
| OMP_PAR_SIMD | 500000 | 1 | 0.000087 | 0.000022 |
| OMP_PAR_SIMD | 500000 | 2 | 0.000138 | 0.000023 |
| OMP_PAR_SIMD | 500000 | 4 | 0.000172 | 0.000031 |
| OMP_PAR_SIMD | 500000 | 8 | 0.000261 | 0.000046 |
| OMP_PAR_SIMD | 500000 | 16 | 0.000864 | 0.001015 |
| OMP_PAR_SIMD | 1000000 | 1 | 0.000243 | 0.000044 |
| OMP_PAR_SIMD | 1000000 | 2 | 0.000262 | 0.000078 |
| OMP_PAR_SIMD | 1000000 | 4 | 0.000239 | 0.000027 |
| OMP_PAR_SIMD | 1000000 | 8 | 0.000322 | 0.000054 |
| OMP_PAR_SIMD | 1000000 | 16 | 0.004138 | 0.005412 |
| OMP_SIMD | 100000 | 1 | 0.000014 | 0.000004 |
| OMP_SIMD | 500000 | 1 | 0.000106 | 0.000025 |
| OMP_SIMD | 1000000 | 1 | 0.000290 | 0.000047 |
| OMP_SMART | 100000 | 1 | 0.000027 | 0.000006 |
| OMP_SMART | 100000 | 2 | 0.000015 | 0.000003 |
| OMP_SMART | 100000 | 4 | 0.000010 | 0.000001 |
| OMP_SMART | 100000 | 8 | 0.000007 | 0.000001 |
| OMP_SMART | 100000 | 16 | 0.000933 | 0.001739 |
| OMP_SMART | 500000 | 1 | 0.000159 | 0.000021 |
| OMP_SMART | 500000 | 2 | 0.000103 | 0.000031 |
| OMP_SMART | 500000 | 4 | 0.000064 | 0.000004 |
| OMP_SMART | 500000 | 8 | 0.000064 | 0.000004 |
| OMP_SMART | 500000 | 16 | 0.000055 | 0.000009 |
| OMP_SMART | 1000000 | 1 | 0.000397 | 0.000061 |
| OMP_SMART | 1000000 | 2 | 0.000253 | 0.000037 |
| OMP_SMART | 1000000 | 4 | 0.000224 | 0.000051 |
| OMP_SMART | 1000000 | 8 | 0.000188 | 0.000018 |
| OMP_SMART | 1000000 | 16 | 0.006777 | 0.002126 |
| SEQ | 100000 | 1 | 0.000015 | 0.000003 |
| SEQ | 500000 | 1 | 0.000119 | 0.000041 |
| SEQ | 1000000 | 1 | 0.000304 | 0.000056 |