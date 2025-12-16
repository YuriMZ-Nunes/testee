import pandas as pd
import matplotlib.pyplot as plt
import os

# Configurações estéticas
plt.style.use('bmh') # Um estilo visual limpo
OUTPUT_DIR = 'results'
CSV_FILE = os.path.join(OUTPUT_DIR, 'dados.csv')

def load_and_process_data():
    if not os.path.exists(CSV_FILE):
        print(f"Erro: Arquivo {CSV_FILE} não encontrado. Rode 'make run' primeiro.")
        return None

    # Lê o CSV
    df = pd.read_csv(CSV_FILE)

    # Agrupa por Versão, N e Threads, calculando média e desvio padrão
    df_agg = df.groupby(['Version', 'N', 'Threads'])['Time'].agg(['mean', 'std']).reset_index()
    return df_agg

def plot_task_c_speedup(df):
    """
    Gera gráfico de Speedup para a Tarefa C (Vetorização + Threads)
    Speedup = Tempo Sequencial / Tempo Paralelo
    """
    print("Gerando gráfico da Tarefa C (Speedup)...")

    # Filtra dados
    df_seq = df[df['Version'] == 'SEQ']
    df_par = df[df['Version'] == 'OMP_PAR_SIMD']

    unique_ns = df['N'].unique()

    plt.figure(figsize=(10, 6))

    for n in sorted(unique_ns):
        # Pega o tempo base (Sequencial) para esse N
        base_time = df_seq[df_seq['N'] == n]['mean'].values[0]

        # Pega os dados da versão paralela para esse N
        subset = df_par[df_par['N'] == n].sort_values('Threads')

        # Calcula Speedup
        speedup = base_time / subset['mean']

        # Plota linha
        plt.plot(subset['Threads'], speedup, marker='o', label=f'N = {n}')

    # Linha de escalabilidade ideal (linear)
    plt.plot([1, 16], [1, 16], 'k--', alpha=0.3, label='Ideal Linear')

    plt.title('Tarefa C: Escalabilidade (Parallel SIMD vs Sequencial)')
    plt.xlabel('Número de Threads')
    plt.ylabel('Speedup (x vezes mais rápido)')
    plt.legend()
    plt.grid(True)
    plt.xticks([1, 2, 4, 8, 16])

    plt.savefig(os.path.join(OUTPUT_DIR, 'grafico_tarefa_c_speedup.png'))
    plt.close()

def plot_task_d_overhead(df):
    """
    Gera gráfico comparativo de Tempo Absoluto para Tarefa D
    Mostra a diferença entre criar threads várias vezes vs uma vez.
    """
    print("Gerando gráfico da Tarefa D (Overhead)...")

    df_naive = df[df['Version'] == 'OMP_NAIVE']
    df_smart = df[df['Version'] == 'OMP_SMART']

    # Vamos focar no N intermediário ou maior para visualização, 
    # ou fazer subplots. Vamos fazer um gráfico para o N maior (ex: 500k ou 1M)
    # onde o overhead é visível mas não dominado totalmente pelo cálculo.
    target_n = 500000 
    if target_n not in df['N'].unique():
        target_n = df['N'].max()

    subset_naive = df_naive[df_naive['N'] == target_n].sort_values('Threads')
    subset_smart = df_smart[df_smart['N'] == target_n].sort_values('Threads')

    plt.figure(figsize=(10, 6))

    # Plota Naive
    plt.errorbar(subset_naive['Threads'], subset_naive['mean'], 
                 yerr=subset_naive['std'], fmt='-o', label='Ingênua (Múltiplos Parallel)', capsize=5)

    # Plota Smart
    plt.errorbar(subset_smart['Threads'], subset_smart['mean'], 
                 yerr=subset_smart['std'], fmt='-s', label='Arrumada (Único Parallel)', capsize=5)

    plt.title(f'Tarefa D: Comparação de Overhead (N={target_n})')
    plt.xlabel('Número de Threads')
    plt.ylabel('Tempo de Execução (s)')
    plt.legend()
    plt.grid(True)
    plt.xticks([1, 2, 4, 8, 16])

    # Escala log pode ajudar se a diferença for brutal,
    # mas escala linear mostra melhor a "distância" visual.
    # plt.yscale('log')

    plt.savefig(os.path.join(OUTPUT_DIR, 'grafico_tarefa_d_overhead.png'))
    plt.close()

def plot_task_c_simd_impact(df):
    """
    Gráfico extra: Impacto da Vetorização pura (1 Thread)
    SEQ vs OMP_SIMD
    """
    print("Gerando gráfico da Tarefa C (Impacto SIMD)...")

    df_seq = df[df['Version'] == 'SEQ']
    df_simd = df[df['Version'] == 'OMP_SIMD']

    unique_ns = df['N'].unique()

    ns = []
    speedups = []

    for n in sorted(unique_ns):
        seq_time = df_seq[df_seq['N'] == n]['mean'].values[0]
        # OMP_SIMD roda com 1 thread no csv (ou ignoramos a thread)
        simd_time = df_simd[(df_simd['N'] == n)]['mean'].values[0]

        speedups.append(seq_time / simd_time)
        ns.append(str(n))

    plt.figure(figsize=(8, 5))
    plt.bar(ns, speedups, color='#4c72b0')
    plt.axhline(y=1.0, color='r', linestyle='--')

    plt.title('Ganho da Vetorização (SIMD) em Single Thread')
    plt.xlabel('Tamanho do Vetor (N)')
    plt.ylabel('Speedup sobre Sequencial')
    plt.ylim(bottom=0.5) # Ajuste visual

    plt.savefig(os.path.join(OUTPUT_DIR, 'grafico_tarefa_c_simd_impact.png'))
    plt.close()

def print_markdown_table(df):
    """
    Gera uma tabela em formato Markdown para copiar e colar no relatório.
    Formato: Média ± Desvio Padrão
    """
    print("\n=== Tabela de Resultados (Copie para RESULTADOS.md) ===")
    print("| Versão | N | Threads | Tempo Médio (s) | Desvio Padrão (s) |")
    print("|---|---|---|---|---|")

    # Itera sobre as linhas do dataframe agregado
    for index, row in df.iterrows():
        versao = row['Version']
        n = row['N']
        threads = row['Threads']
        media = row['mean']
        std = row['std']

        # Formata com 6 casas decimais e notação científica se for muito pequeno
        print(f"| {versao} | {n} | {threads} | {media:.6f} | {std:.6f} |")
    print("=======================================================\n")

if __name__ == "__main__":
    df = load_and_process_data()
    if df is not None:
        print_markdown_table(df)
        plot_task_c_speedup(df)
        plot_task_d_overhead(df)
        plot_task_c_simd_impact(df)
        print("Gráficos gerados na pasta 'results/'")