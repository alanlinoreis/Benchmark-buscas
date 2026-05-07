import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker
import numpy as np
import os

# ============================================================
# CONFIGURAÇÃO — altere aqui ao trocar de base de dados
# ============================================================
# Deve coincidir com o vetor TAMANHOS definido no main.cpp
TAMANHOS_ESPERADOS = [50, 100, 150, 200, 250, 310]

# ============================================================
# 1. CARREGAR DADOS
# ============================================================

def carregar(caminho):
    if not os.path.exists(caminho):
        print(f"Arquivo não encontrado: {caminho}")
        return None
    return pd.read_csv(caminho)

df_ord = carregar('resultados_ordenacao.csv')
df_bus = carregar('resultados_busca.csv')

if df_ord is None or df_bus is None:
    print("Rode o programa C++ primeiro para gerar os arquivos CSV.")
    exit()

# Remove o algoritmo auxiliar de ordenação decrescente dos gráficos
df_ord = df_ord[df_ord['Algoritmo'] != 'InsertionDesc'].copy()

# ============================================================
# 2. CONFIGURAÇÕES VISUAIS
# ============================================================

plt.style.use('ggplot')

CORES     = ['#e6194B', '#3cb44b', '#4363d8', '#f58231', '#911eb4', '#42d4f4', '#9a6324', '#000075', '#469990']
MARCAS    = ['o', 's', '^', 'D', 'v', 'X', 'P', '*', 'h']
LINHAS    = ['-', '--', '-.', ':', '-', '--', '-.', ':', '-']
GROSSURAS = [2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5]

def estilo(j):
    return dict(
        color=CORES[j % len(CORES)],
        marker=MARCAS[j % len(MARCAS)],
        linestyle=LINHAS[j % len(LINHAS)],
        linewidth=GROSSURAS[j % len(GROSSURAS)],
        markersize=6,
        alpha=0.9
    )

def sem_notacao_cientifica(ax):
    ax.yaxis.set_major_formatter(mticker.ScalarFormatter())
    ax.ticklabel_format(style='plain', axis='y')

def salvar(nome):
    plt.tight_layout()
    plt.savefig(nome, dpi=150, bbox_inches='tight')
    plt.close()
    print(f"Gráfico salvo: {nome}")

algoritmos = df_ord['Algoritmo'].unique()

ROTULOS_FASE = {
    'Desordenado->Crescente':  'Entrada Desordenada',
    'Ordenado->Crescente':     'Já Ordenado (Crescente)',
    'Decrescente->Crescente':  'Entrada Decrescente',
}

ROTULOS_ALVO = {0: 'Alvo início', 1: 'Alvo meio', 2: 'Alvo fim'}

# ============================================================
# 3. GRÁFICOS DE ORDENAÇÃO
#    Um arquivo por métrica; 3 sub-gráficos (uma fase cada).
#    Eixo X = N, eixo Y = métrica, uma linha por algoritmo.
# ============================================================

metricas_ord = [
    ('Comparacoes', 'Comparações'),
    ('Trocas',      'Trocas'),
    ('Tempo(s)',    'Tempo de Execução (s)'),
]

fases_ord = list(ROTULOS_FASE.keys())

for col_key, col_label in metricas_ord:
    fig, axes = plt.subplots(1, 3, figsize=(21, 6), sharey=False)
    fig.suptitle(f'Ordenação — {col_label} × Tamanho da Entrada', fontsize=15, fontweight='bold')

    for col, fase in enumerate(fases_ord):
        ax = axes[col]
        df_fase = df_ord[df_ord['Fase'] == fase]

        for j, alg in enumerate(algoritmos):
            df_alg = df_fase[df_fase['Algoritmo'] == alg].sort_values('N')
            if df_alg.empty:
                continue
            ax.plot(df_alg['N'], df_alg[col_key], label=alg, **estilo(j))

        ax.set_title(ROTULOS_FASE[fase], fontsize=12)
        ax.set_xlabel('Número de Registros (N)', fontsize=10)
        ax.set_ylabel(col_label, fontsize=10)
        ax.legend(fontsize=9, loc='upper left')
        ax.grid(True, alpha=0.4)
        sem_notacao_cientifica(ax)

    nome = f"ord_{col_key.replace('(','').replace(')','').replace('/','').lower()}.png"
    salvar(nome)


# ============================================================
# 4. GRÁFICOS DE BUSCA — COMPARAÇÕES
#    4a: Busca Sequencial por fase (3 linhas = 3 alvos)
#    4b: Sequencial vs Binária na fase Ordenado_Crescente
# ============================================================

fases_busca_seq = ['Desordenado', 'Ordenado_Crescente', 'Ordenado_Decrescente']
ROTULOS_FASE_BUS = {
    'Desordenado':          'Desordenado',
    'Ordenado_Crescente':   'Ordenado Crescente',
    'Ordenado_Decrescente': 'Ordenado Decrescente',
}

def grafico_busca_comparacoes():
    # --- 4a: Sequencial, comparações, por fase e alvo ---
    fig, axes = plt.subplots(1, 3, figsize=(21, 6), sharey=False)
    fig.suptitle('Busca Sequencial — Comparações × N', fontsize=15, fontweight='bold')

    df_seq = df_bus[df_bus['TipoBusca'] == 'Sequencial']

    for col, fase in enumerate(fases_busca_seq):
        ax = axes[col]
        df_fase = df_seq[df_seq['Fase'] == fase]

        for j, pos in enumerate(sorted(df_fase['AlvoPos'].unique())):
            df_alvo = df_fase[df_fase['AlvoPos'] == pos].sort_values('N')
            if df_alvo.empty:
                continue
            ax.plot(df_alvo['N'], df_alvo['Comparacoes'],
                    label=ROTULOS_ALVO[pos], **estilo(j))

        ax.set_title(ROTULOS_FASE_BUS[fase], fontsize=12)
        ax.set_xlabel('Número de Registros (N)', fontsize=10)
        ax.set_ylabel('Comparações', fontsize=10)
        ax.legend(fontsize=9)
        ax.grid(True, alpha=0.4)
        sem_notacao_cientifica(ax)

    salvar('busca_seq_comparacoes.png')

    # --- 4b: Sequencial vs Binária vs HashDuplo, dados ordenados crescente ---
    fig, axes = plt.subplots(1, 3, figsize=(21, 6), sharey=False)
    fig.suptitle('Seq. vs Binária vs HashDuplo (Ordenado Crescente) — Comparações × N',
                 fontsize=15, fontweight='bold')

    df_orc = df_bus[df_bus['Fase'] == 'Ordenado_Crescente']

    for col, pos in enumerate(sorted(df_orc['AlvoPos'].unique())):
        ax = axes[col]
        df_pos = df_orc[df_orc['AlvoPos'] == pos]

        for j, tipo in enumerate(['Sequencial', 'Binaria', 'HashDuplo']):
            df_tipo = df_pos[df_pos['TipoBusca'] == tipo].sort_values('N')
            if df_tipo.empty:
                continue
            ax.plot(df_tipo['N'], df_tipo['Comparacoes'],
                    label=tipo, **estilo(j))

        ax.set_title(f'Alvo: {ROTULOS_ALVO[pos]}', fontsize=12)
        ax.set_xlabel('Número de Registros (N)', fontsize=10)
        ax.set_ylabel('Comparações', fontsize=10)
        ax.legend(fontsize=9)
        ax.grid(True, alpha=0.4)
        sem_notacao_cientifica(ax)

    salvar('busca_seq_vs_bin_comparacoes.png')

    # --- 4c: HashDuplo — comparações por fase e alvo ---
    fig, axes = plt.subplots(1, 3, figsize=(21, 6), sharey=False)
    fig.suptitle('HashDuplo — Comparações × N (por fase)', fontsize=15, fontweight='bold')

    df_hash = df_bus[df_bus['TipoBusca'] == 'HashDuplo']

    for col, fase in enumerate(fases_busca_seq):
        ax = axes[col]
        df_fase = df_hash[df_hash['Fase'] == fase]

        for j, pos in enumerate(sorted(df_fase['AlvoPos'].unique())):
            df_alvo = df_fase[df_fase['AlvoPos'] == pos].sort_values('N')
            if df_alvo.empty:
                continue
            ax.plot(df_alvo['N'], df_alvo['Comparacoes'],
                    label=ROTULOS_ALVO[pos], **estilo(j))

        ax.set_title(ROTULOS_FASE_BUS[fase], fontsize=12)
        ax.set_xlabel('Número de Registros (N)', fontsize=10)
        ax.set_ylabel('Comparações (sondagens)', fontsize=10)
        ax.legend(fontsize=9)
        ax.grid(True, alpha=0.4)
        sem_notacao_cientifica(ax)

    salvar('busca_hash_comparacoes.png')

grafico_busca_comparacoes()


# ============================================================
# 5. GRÁFICOS DE BUSCA — TEMPO
# ============================================================

def grafico_busca_tempo():
    # --- 5a: Sequencial, tempo, por fase e alvo ---
    fig, axes = plt.subplots(1, 3, figsize=(21, 6), sharey=False)
    fig.suptitle('Busca Sequencial — Tempo × N', fontsize=15, fontweight='bold')

    df_seq = df_bus[df_bus['TipoBusca'] == 'Sequencial']

    for col, fase in enumerate(fases_busca_seq):
        ax = axes[col]
        df_fase = df_seq[df_seq['Fase'] == fase]

        for j, pos in enumerate(sorted(df_fase['AlvoPos'].unique())):
            df_alvo = df_fase[df_fase['AlvoPos'] == pos].sort_values('N')
            if df_alvo.empty:
                continue
            ax.plot(df_alvo['N'], df_alvo['Tempo(s)'],
                    label=ROTULOS_ALVO[pos], **estilo(j))

        ax.set_title(ROTULOS_FASE_BUS[fase], fontsize=12)
        ax.set_xlabel('Número de Registros (N)', fontsize=10)
        ax.set_ylabel('Tempo (s)', fontsize=10)
        ax.legend(fontsize=9)
        ax.grid(True, alpha=0.4)
        sem_notacao_cientifica(ax)

    salvar('busca_seq_tempo.png')

    # --- 5b: Sequencial vs Binária vs HashDuplo, tempo, dados ordenados crescente ---
    fig, axes = plt.subplots(1, 3, figsize=(21, 6), sharey=False)
    fig.suptitle('Seq. vs Binária vs HashDuplo (Ordenado Crescente) — Tempo × N',
                 fontsize=15, fontweight='bold')

    df_orc = df_bus[df_bus['Fase'] == 'Ordenado_Crescente']

    for col, pos in enumerate(sorted(df_orc['AlvoPos'].unique())):
        ax = axes[col]
        df_pos = df_orc[df_orc['AlvoPos'] == pos]

        for j, tipo in enumerate(['Sequencial', 'Binaria', 'HashDuplo']):
            df_tipo = df_pos[df_pos['TipoBusca'] == tipo].sort_values('N')
            if df_tipo.empty:
                continue
            ax.plot(df_tipo['N'], df_tipo['Tempo(s)'],
                    label=tipo, **estilo(j))

        ax.set_title(f'Alvo: {ROTULOS_ALVO[pos]}', fontsize=12)
        ax.set_xlabel('Número de Registros (N)', fontsize=10)
        ax.set_ylabel('Tempo (s)', fontsize=10)
        ax.legend(fontsize=9)
        ax.grid(True, alpha=0.4)
        sem_notacao_cientifica(ax)

    salvar('busca_seq_vs_bin_tempo.png')

    # --- 5c: HashDuplo — tempo por fase e alvo (inclui custo de construção) ---
    fig, axes = plt.subplots(1, 3, figsize=(21, 6), sharey=False)
    fig.suptitle('HashDuplo — Tempo × N por fase (construção + busca)',
                 fontsize=15, fontweight='bold')

    df_hash = df_bus[df_bus['TipoBusca'] == 'HashDuplo']

    for col, fase in enumerate(fases_busca_seq):
        ax = axes[col]
        df_fase = df_hash[df_hash['Fase'] == fase]

        for j, pos in enumerate(sorted(df_fase['AlvoPos'].unique())):
            df_alvo = df_fase[df_fase['AlvoPos'] == pos].sort_values('N')
            if df_alvo.empty:
                continue
            ax.plot(df_alvo['N'], df_alvo['Tempo(s)'],
                    label=ROTULOS_ALVO[pos], **estilo(j))

        ax.set_title(ROTULOS_FASE_BUS[fase], fontsize=12)
        ax.set_xlabel('Número de Registros (N)', fontsize=10)
        ax.set_ylabel('Tempo (s)', fontsize=10)
        ax.legend(fontsize=9)
        ax.grid(True, alpha=0.4)
        sem_notacao_cientifica(ax)

    salvar('busca_hash_tempo.png')

grafico_busca_tempo()


print("\nTodos os gráficos foram gerados com sucesso!")
print("Arquivos gerados:")
for f in [
    'ord_comparacoes.png',
    'ord_trocas.png',
    'ord_tempos.png',
    'busca_seq_comparacoes.png',
    'busca_seq_vs_bin_comparacoes.png',
    'busca_hash_comparacoes.png',
    'busca_seq_tempo.png',
    'busca_seq_vs_bin_tempo.png',
    'busca_hash_tempo.png',
]:
    print(f"  {f}")
