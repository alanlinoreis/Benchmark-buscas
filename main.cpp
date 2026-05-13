#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <functional>
#include <ctime>
#include <iomanip>
#include <algorithm>

using namespace std;

// =========================================================================
// CONFIGURAÇÃO DE DADOS
// =========================================================================
// Arquivo CSV padrão a ser utilizado (altere aqui para trocar a base de dados)
const string ARQUIVO_CSV_PADRAO = "dados-cadastrais-revendedores-varejistas-combustiveis-automoveis.csv";

// Índice de coluna padrão para ordenação/busca (0 = primeira coluna)
// Altere conforme necessário para usar uma coluna diferente
const int INDICE_COLUNA_PADRAO = 0;

// =========================================================================
// TAMANHOS DE SUBCONJUNTO
// Altere este vetor para adaptar a bases maiores.
// O último valor deve ser <= total de registros do CSV.
// Exemplo para 50 000 registros: {1000, 5000, 10000, 20000, 35000, 50000}
// =========================================================================
vector<int> TAMANHOS = {1000, 5000, 10000, 20000, 35000, 46044};

// =========================================================================
// ESTRUTURA DE DADOS
// =========================================================================

struct Registro {
    long long valor;  // valor numérico da coluna (armazenado como inteiro * 1000)
};

// =========================================================================
// 1. ALGORITMOS DE ORDENAÇÃO
// =========================================================================

void bubbleSortFlag(vector<Registro> &arr, long long &comparacoes, long long &trocas) {
    int n = arr.size();
    comparacoes = 0; trocas = 0;
    bool trocou;
    for (int i = 0; i < n - 1; i++) {
        trocou = false;
        for (int j = 0; j < n - 1 - i; j++) {
            comparacoes++;
            if (arr[j].valor > arr[j + 1].valor) {
                trocou = true;
                swap(arr[j], arr[j + 1]);
                trocas++;
            }
        }
        if (!trocou) break;
    }
}

void insertionSort(vector<Registro> &arr, long long &comparacoes, long long &trocas) {
    int n = arr.size();
    comparacoes = 0; trocas = 0;
    for (int i = 1; i < n; i++) {
        Registro chave = arr[i];
        int j = i - 1;
        while (j >= 0) {
            comparacoes++;
            if (arr[j].valor > chave.valor) {
                arr[j + 1] = arr[j];
                trocas++;
                j--;
            } else break;
        }
        arr[j + 1] = chave;
    }
}

void selectionSort(vector<Registro> &arr, long long &comparacoes, long long &trocas) {
    int n = arr.size();
    comparacoes = 0; trocas = 0;
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            comparacoes++;
            if (arr[j].valor < arr[min_idx].valor)
                min_idx = j;
        }
        if (min_idx != i) { swap(arr[min_idx], arr[i]); trocas++; }
    }
}

void shellSort(vector<Registro> &arr, long long &comparacoes, long long &trocas) {
    int n = arr.size();
    comparacoes = 0; trocas = 0;
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int j = i;
            while (j >= gap) {
                comparacoes++;
                if (arr[j - gap].valor > arr[j].valor) {
                    swap(arr[j], arr[j - gap]);
                    trocas++;
                    j -= gap;
                } else break;
            }
        }
    }
}

int particionaLomuto(vector<Registro> &arr, int ini, int fim,
                     long long &comparacoes, long long &trocas) {
    long long pivo = arr[fim].valor;
    int i = ini - 1;
    for (int j = ini; j < fim; j++) {
        comparacoes++;
        if (arr[j].valor <= pivo) { i++; swap(arr[i], arr[j]); trocas++; }
    }
    swap(arr[i + 1], arr[fim]); trocas++;
    return i + 1;
}

void quickSortLomuto(vector<Registro> &arr, long long &comparacoes, long long &trocas) {
    comparacoes = 0; trocas = 0;
    if (arr.empty()) return;
    vector<pair<int,int>> pilha;
    pilha.push_back({0, (int)arr.size() - 1});
    while (!pilha.empty()) {
        auto [ini, fim] = pilha.back(); pilha.pop_back();
        if (ini < fim) {
            int idx = particionaLomuto(arr, ini, fim, comparacoes, trocas);
            pilha.push_back({ini, idx - 1});
            pilha.push_back({idx + 1, fim});
        }
    }
}

int particionaHoare(vector<Registro> &arr, int ini, int fim,
                    long long &comparacoes, long long &trocas) {
    long long pivo = arr[ini].valor;
    int i = ini - 1, j = fim + 1;
    while (true) {
        do { i++; comparacoes++; } while (arr[i].valor < pivo);
        do { j--; comparacoes++; } while (arr[j].valor > pivo);
        if (i >= j) return j;
        swap(arr[i], arr[j]); trocas++;
    }
}

void quickSortHoare(vector<Registro> &arr, long long &comparacoes, long long &trocas) {
    comparacoes = 0; trocas = 0;
    if (arr.empty()) return;
    vector<pair<int,int>> pilha;
    pilha.push_back({0, (int)arr.size() - 1});
    while (!pilha.empty()) {
        auto [ini, fim] = pilha.back(); pilha.pop_back();
        if (ini < fim) {
            int idx = particionaHoare(arr, ini, fim, comparacoes, trocas);
            pilha.push_back({ini, idx});
            pilha.push_back({idx + 1, fim});
        }
    }
}

// -------------------------------------------------------------------------
// Merge Sort
// -------------------------------------------------------------------------

void mergeSortMerge(vector<Registro> &arr, int esq, int meio, int dir,
                    long long &comparacoes, long long &trocas) {
    vector<Registro> tmp(arr.begin() + esq, arr.begin() + dir + 1);
    int i = 0, j = meio - esq + 1, k = esq;
    int tam_esq = meio - esq + 1;
    while (i < tam_esq && j < (dir - esq + 1)) {
        comparacoes++;
        if (tmp[i].valor <= tmp[j].valor) {
            arr[k++] = tmp[i++];
        } else {
            arr[k++] = tmp[j++];
            trocas += tam_esq - i; // conta deslocamentos como "trocas"
        }
    }
    while (i < tam_esq)  arr[k++] = tmp[i++];
    while (j < (dir - esq + 1)) arr[k++] = tmp[j++];
}

void mergeSortRec(vector<Registro> &arr, int esq, int dir,
                  long long &comparacoes, long long &trocas) {
    if (esq >= dir) return;
    int meio = esq + (dir - esq) / 2;
    mergeSortRec(arr, esq, meio, comparacoes, trocas);
    mergeSortRec(arr, meio + 1, dir, comparacoes, trocas);
    mergeSortMerge(arr, esq, meio, dir, comparacoes, trocas);
}

void mergeSort(vector<Registro> &arr, long long &comparacoes, long long &trocas) {
    comparacoes = 0; trocas = 0;
    if (!arr.empty()) mergeSortRec(arr, 0, arr.size() - 1, comparacoes, trocas);
}

// -------------------------------------------------------------------------
// Radix Sort (versão simples e segura)
// -------------------------------------------------------------------------

void radixSort(vector<Registro> &arr, long long &comparacoes, long long &trocas) {
    comparacoes = 0; trocas = 0;
    if (arr.empty()) return;

    int n = arr.size();
    vector<Registro> aux(n);  // Um único buffer auxiliar reutilizável
    
    // Encontrar máximo valor
    long long maxVal = 0;
    for (auto &reg : arr) {
        if (reg.valor > maxVal) maxVal = reg.valor;
    }
    
    // Radix sort para valores positivos apenas
    if (maxVal == 0) return;  // Tudo é zero
    
    for (long long exp = 1; maxVal / exp > 0; exp *= 10) {
        int contagem[10] = {0};
        
        // Contar dígitos
        for (int i = 0; i < n; i++) {
            comparacoes++;
            int digito = (int)((arr[i].valor / exp) % 10);
            contagem[digito]++;
        }
        
        // Converter para índices
        for (int i = 1; i < 10; i++) {
            contagem[i] += contagem[i - 1];
        }
        
        // Colocar no array auxiliar
        for (int i = n - 1; i >= 0; i--) {
            int digito = (int)((arr[i].valor / exp) % 10);
            aux[--contagem[digito]] = arr[i];
            trocas++;
        }
        
        // Copiar de volta
        swap(arr, aux);
    }
}

void insertionSortDesc(vector<Registro> &arr, long long &comparacoes, long long &trocas) {
    int n = arr.size();
    comparacoes = 0; trocas = 0;
    for (int i = 1; i < n; i++) {
        Registro chave = arr[i];
        int j = i - 1;
        while (j >= 0) {
            comparacoes++;
            if (arr[j].valor < chave.valor) {
                arr[j + 1] = arr[j]; trocas++; j--;
            } else break;
        }
        arr[j + 1] = chave;
    }
}

// =========================================================================
// 2. HEAP SORT
// =========================================================================

// HEAPIFY (sift-down): corrige violação local no nó i, assumindo que
// as sub-árvores esq(i) e dir(i) já são max-heaps válidos.
// Parâmetros: array, tamanho efetivo n, índice do nó a corrigir.
void heapify(vector<Registro> &arr, int n, int i,
             long long &comparacoes, long long &trocas) {
    int maior = i;
    int esq   = 2 * i + 1;
    int dir   = 2 * i + 2;

    if (esq < n) {
        comparacoes++;
        if (arr[esq].valor > arr[maior].valor)
            maior = esq;
    }

    if (dir < n) {
        comparacoes++;
        if (arr[dir].valor > arr[maior].valor)
            maior = dir;
    }

    if (maior != i) {
        swap(arr[i], arr[maior]);
        trocas++;
        heapify(arr, n, maior, comparacoes, trocas); // afunda recursivamente
    }
}

void heapSort(vector<Registro> &arr, long long &comparacoes, long long &trocas) {
    comparacoes = 0; trocas = 0;
    int n = arr.size();
    if (n <= 1) return;

    // BUILD-MAX-HEAP: processa apenas nós internos (folhas já são heaps triviais).
    // Primeiro nó interno = índice n/2 - 1; processa de baixo para cima (bottom-up).
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i, comparacoes, trocas);

    // Extração: troca a raiz (maior) com o último elemento do heap,
    // reduz o tamanho efetivo em 1 e restaura a propriedade com heapify.
    // Atenção: passa i como tamanho — não n — para ignorar a parte já ordenada.
    for (int i = n - 1; i >= 1; i--) {
        swap(arr[0], arr[i]);
        trocas++;
        heapify(arr, i, 0, comparacoes, trocas);
    }
}

// =========================================================================
// 3. HASH DUPLO
//    Endereçamento aberto com duas funções hash.
//    O tamanho da tabela é o menor primo >= 2*n para manter fator de carga
//    abaixo de 0.5, o que garante desempenho adequado.
// =========================================================================

enum EstadoPosicao { VAZIO, OCUPADO, REMOVIDO };

// Retorna true se p é primo
bool ehPrimo(int p) {
    if (p < 2) return false;
    for (int i = 2; i * i <= p; i++)
        if (p % i == 0) return false;
    return true;
}

// Menor primo >= n
int proximoPrimo(int n) {
    while (!ehPrimo(n)) n++;
    return n;
}

class HashDuplo {
private:
    int tamanho;
    vector<long long>     chaves;        // chave = valor
    vector<int>           indices;       // índice original no vetor de dados
    vector<EstadoPosicao> estado;

    int hash1(long long chave) const { return ((chave % tamanho) + tamanho) % tamanho; }
    int hash2(long long chave) const { return 1 + (chave % (tamanho - 1)); }

public:
    HashDuplo(int tam)
        : tamanho(tam), chaves(tam, 0), indices(tam, -1), estado(tam, VAZIO) {}

    void inserir(long long chave, int idx_original) {
        int h1 = hash1(chave);
        int h2 = hash2(chave);
        for (int i = 0; i < tamanho; i++) {
            int pos = (h1 + i * h2) % tamanho;
            if (estado[pos] == VAZIO || estado[pos] == REMOVIDO) {
                chaves[pos]  = chave;
                indices[pos] = idx_original;
                estado[pos]  = OCUPADO;
                return;
            }
        }
    }

    int buscar(long long chave, long long &comparacoes) const {
        int h1 = hash1(chave);
        int h2 = hash2(chave);
        for (int i = 0; i < tamanho; i++) {
            int pos = (h1 + i * h2) % tamanho;
            comparacoes++;
            if (estado[pos] == VAZIO)                          return -1;
            if (estado[pos] == OCUPADO && chaves[pos] == chave) return indices[pos];
        }
        return -1;
    }
};

// Constrói a tabela desde o início (tempo conta a partir da criação)
// e realiza a busca. Retorna o índice encontrado ou -1.
int buscaHashDuplo(const vector<Registro> &arr, long long alvo,
                   long long &comparacoes, double &tempo_s) {
    int n       = arr.size();
    int tam_tab = proximoPrimo(2 * n);  // fator de carga < 0.5

    clock_t ini = clock();              // inicia ANTES de criar a tabela

    HashDuplo ht(tam_tab);
    for (int i = 0; i < n; i++)
        ht.inserir(arr[i].valor, i);

    comparacoes = 0;
    int resultado = ht.buscar(alvo, comparacoes);

    clock_t fim = clock();
    tempo_s = (double)(fim - ini) / CLOCKS_PER_SEC;

    return resultado;
}

// =========================================================================
// 4. ALGORITMOS DE BUSCA CLÁSSICOS
// =========================================================================

int buscaSequencial(const vector<Registro> &arr, long long alvo, long long &comparacoes) {
    comparacoes = 0;
    for (int i = 0; i < (int)arr.size(); i++) {
        comparacoes++;
        if (arr[i].valor == alvo) return i;
    }
    return -1;
}

int buscaBinaria(const vector<Registro> &arr, long long alvo, long long &comparacoes) {
    comparacoes = 0;
    int esq = 0, dir = arr.size() - 1;
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;
        comparacoes++;
        if (arr[meio].valor == alvo) return meio;
        else if (arr[meio].valor < alvo) esq = meio + 1;
        else dir = meio - 1;
    }
    return -1;
}

// =========================================================================
// 3. LEITURA DO CSV (GENÉRICA)
// =========================================================================

// Detecta o separador do CSV (vírgula ou ponto-e-vírgula)
char detectarSeparador(const string &linha) {
    int contagem_virgula = 0, contagem_ponto_virgula = 0;
    for (char c : linha) {
        if (c == ',') contagem_virgula++;
        if (c == ';') contagem_ponto_virgula++;
    }
    return (contagem_ponto_virgula > contagem_virgula) ? ';' : ',';
}

// Divide uma linha usando o separador fornecido
vector<string> dividirLinha(const string &linha, char separador) {
    vector<string> campos;
    istringstream ss(linha);
    string campo;
    while (getline(ss, campo, separador)) {
        campos.push_back(campo);
    }
    return campos;
}

// Remove espaços em branco do início e fim da string
string trim(const string &str) {
    size_t inicio = str.find_first_not_of(" \t\r\n");
    if (inicio == string::npos) return "";
    size_t fim = str.find_last_not_of(" \t\r\n");
    return str.substr(inicio, fim - inicio + 1);
}

// Lê CSV com suporte a múltiplos separadores e índice de coluna configurável
vector<Registro> lerCSV(const string &caminho, string &cabecalho, int indice_coluna = 0) {
    vector<Registro> dados;
    ifstream arquivo(caminho);
    if (!arquivo.is_open()) { cerr << "Erro: não foi possível abrir " << caminho << endl; exit(1); }
    
    // Lê cabeçalho e detecta separador
    getline(arquivo, cabecalho);
    char separador = detectarSeparador(cabecalho);
    
    cerr << "Separador detectado: '" << separador << "'" << endl;
    
    // Valida índice da coluna
    vector<string> colunas = dividirLinha(cabecalho, separador);
    cerr << "Total de colunas: " << colunas.size() << endl;
    if (indice_coluna < 0 || indice_coluna >= (int)colunas.size()) {
        cerr << "Aviso: índice de coluna " << indice_coluna << " inválido. Usando coluna 0." << endl;
        indice_coluna = 0;
    }
    cerr << "Usando coluna " << indice_coluna << ": " << trim(colunas[indice_coluna]) << endl;
    
    string linha;
    int linha_numero = 1;
    while (getline(arquivo, linha)) {
        if (linha.empty()) continue;
        linha_numero++;
        
        vector<string> campos = dividirLinha(linha, separador);
        if (indice_coluna >= (int)campos.size()) continue;
        
        string campo_valor = trim(campos[indice_coluna]);
        try {
            Registro p;
            p.valor = (long long)(stod(campo_valor) * 1000.0);
            dados.push_back(p);
        } catch (...) {
            cerr << "Aviso: linha " << linha_numero << " não pôde ser processada." << endl;
        }
    }
    return dados;
}

// =========================================================================
// 4. ESTRUTURAS DE SAÍDA
// =========================================================================

struct Algoritmo {
    string nome;
    function<void(vector<Registro>&, long long&, long long&)> func;
};;

struct ResultadoOrdenacao {
    string algoritmo;
    string fase;
    int n;
    double tempo_s;
    long long comparacoes;
    long long trocas;
};

struct ResultadoBusca {
    string tipo_busca;
    string fase;
    int n;
    int alvo_pos; // 0=inicio, 1=meio, 2=fim
    double tempo_s;
    long long comparacoes;
    int indice_encontrado;
};

// =========================================================================
// 5. MAIN
// =========================================================================

int main(int argc, char *argv[]) {
    // Uso: ./programa [arquivo.csv] [indice_coluna]
    // Exemplos:
    //   ./programa                                    (usa ARQUIVO_CSV_PADRAO, coluna padrão)
    //   ./programa dados-cadastrais.csv               (usa dados-cadastrais.csv, coluna 0)
    //   ./programa dados-cadastrais.csv 0             (coluna CODIGOISIMP)
    //   ./programa dados-cadastrais.csv 4             (coluna CNPJ)
    // 
    // Dica: Para alterar a base de dados padrão, edite a constante
    //       ARQUIVO_CSV_PADRAO no início do arquivo.
    
    string caminho_csv = (argc > 1) ? argv[1] : ARQUIVO_CSV_PADRAO;
    int indice_coluna = (argc > 2) ? atoi(argv[2]) : INDICE_COLUNA_PADRAO;
    
    string cabecalho;
    
    cerr << "======================================" << endl;
    cerr << "Lendo arquivo: " << caminho_csv << endl;
    cerr << "======================================" << endl;

    vector<Registro> dados_todos = lerCSV(caminho_csv, cabecalho, indice_coluna);
    int total = dados_todos.size();
    cerr << "Registros carregados: " << total << endl;

    // Valida tamanhos — remove os que excedem o total disponível
    vector<int> tamanhos_validos;
    for (int t : TAMANHOS) {
        if (t <= total) tamanhos_validos.push_back(t);
        else cerr << "Aviso: tamanho " << t << " excede o total de dados (" << total << "), ignorado.\n";
    }

    vector<Algoritmo> algoritmos = {
        {"Bubble_Flag",  bubbleSortFlag},
        {"Insertion",    insertionSort},
        {"Selection",    selectionSort},
        {"Shell",        shellSort},
        {"Quick_Lomuto", quickSortLomuto},
        {"Quick_Hoare",  quickSortHoare},
        {"Merge",        mergeSort},
        {"Radix",        radixSort},
        {"Heap",         heapSort}
    };

    vector<ResultadoOrdenacao> resultados_ord;
    vector<ResultadoBusca>     resultados_busca;

    // -----------------------------------------------------------------------
    // Loop por tamanho de subconjunto
    // -----------------------------------------------------------------------
    for (int tam : tamanhos_validos) {
        cerr << "Processando n=" << tam << "..." << endl;

        // Subconjunto dos primeiros `tam` registros (ordem original = desordenada)
        vector<Registro> sub(dados_todos.begin(), dados_todos.begin() + tam);

        // Alvos: índice 0, tam/2, tam-1 (posição relativa ao subconjunto)
        // Guardamos a posição (0, 1, 2) para facilitar rótulos no Python
        vector<pair<int,long long>> alvos = {
            {0, sub[0].valor},
            {1, sub[tam / 2].valor},
            {2, sub[tam - 1].valor}
        };

        // --- Fase A: desordenado → busca sequencial + hash duplo ---
        for (auto &[pos, alvo] : alvos) {
            {
                long long c = 0;
                clock_t ini = clock();
                int idx = buscaSequencial(sub, alvo, c);
                clock_t fim = clock();
                resultados_busca.push_back({"Sequencial", "Desordenado", tam, pos,
                                            (double)(fim - ini) / CLOCKS_PER_SEC, c, idx});
            }
            {
                long long c = 0; double tempo = 0;
                int idx = buscaHashDuplo(sub, alvo, c, tempo);
                resultados_busca.push_back({"HashDuplo", "Desordenado", tam, pos,
                                            tempo, c, idx});
            }
        }

        // --- Fase B: desordenado → ordenação ---
        vector<Registro> sub_ord_asc;
        for (auto &algo : algoritmos) {
            vector<Registro> copia = sub;
            long long c = 0, t = 0;
            clock_t ini = clock();
            algo.func(copia, c, t);
            clock_t fim = clock();
            resultados_ord.push_back({algo.nome, "Desordenado->Crescente", tam,
                                      (double)(fim - ini) / CLOCKS_PER_SEC, c, t});
            if (sub_ord_asc.empty()) sub_ord_asc = copia; // guarda cópia ordenada
        }

        // --- Fase C: já ordenado crescente → ordenação ---
        for (auto &algo : algoritmos) {
            vector<Registro> copia = sub_ord_asc;
            long long c = 0, t = 0;
            clock_t ini = clock();
            algo.func(copia, c, t);
            clock_t fim = clock();
            resultados_ord.push_back({algo.nome, "Ordenado->Crescente", tam,
                                      (double)(fim - ini) / CLOCKS_PER_SEC, c, t});
        }

        // --- Fase D: busca em dados ordenados crescente ---
        for (auto &[pos, alvo] : alvos) {
            {
                long long c = 0;
                clock_t ini = clock();
                int idx = buscaSequencial(sub_ord_asc, alvo, c);
                clock_t fim = clock();
                resultados_busca.push_back({"Sequencial", "Ordenado_Crescente", tam, pos,
                                            (double)(fim - ini) / CLOCKS_PER_SEC, c, idx});
            }
            {
                long long c = 0;
                clock_t ini = clock();
                int idx = buscaBinaria(sub_ord_asc, alvo, c);
                clock_t fim = clock();
                resultados_busca.push_back({"Binaria", "Ordenado_Crescente", tam, pos,
                                            (double)(fim - ini) / CLOCKS_PER_SEC, c, idx});
            }
            {
                long long c = 0; double tempo = 0;
                int idx = buscaHashDuplo(sub_ord_asc, alvo, c, tempo);
                resultados_busca.push_back({"HashDuplo", "Ordenado_Crescente", tam, pos,
                                            tempo, c, idx});
            }
        }

        // --- Fase E: ordenar decrescente ---
        vector<Registro> sub_desc = sub_ord_asc;
        {
            long long c = 0, t = 0;
            clock_t ini = clock();
            insertionSortDesc(sub_desc, c, t);
            clock_t fim = clock();
            resultados_ord.push_back({"InsertionDesc", "Crescente->Decrescente", tam,
                                      (double)(fim - ini) / CLOCKS_PER_SEC, c, t});
        }

        // --- Fase F: busca sequencial + hash duplo em dados decrescentes ---
        for (auto &[pos, alvo] : alvos) {
            {
                long long c = 0;
                clock_t ini = clock();
                int idx = buscaSequencial(sub_desc, alvo, c);
                clock_t fim = clock();
                resultados_busca.push_back({"Sequencial", "Ordenado_Decrescente", tam, pos,
                                            (double)(fim - ini) / CLOCKS_PER_SEC, c, idx});
            }
            {
                long long c = 0; double tempo = 0;
                int idx = buscaHashDuplo(sub_desc, alvo, c, tempo);
                resultados_busca.push_back({"HashDuplo", "Ordenado_Decrescente", tam, pos,
                                            tempo, c, idx});
            }
        }

        // --- Fase G: decrescente → ordenação crescente ---
        for (auto &algo : algoritmos) {
            vector<Registro> copia = sub_desc;
            long long c = 0, t = 0;
            clock_t ini = clock();
            algo.func(copia, c, t);
            clock_t fim = clock();
            resultados_ord.push_back({algo.nome, "Decrescente->Crescente", tam,
                                      (double)(fim - ini) / CLOCKS_PER_SEC, c, t});
        }
    }

    // =========================================================================
    // SAÍDA — resultados_ordenacao.csv
    // =========================================================================
    {
        ofstream out("resultados_ordenacao.csv");
        out << "Algoritmo,Fase,N,Tempo(s),Comparacoes,Trocas\n";
        for (auto &r : resultados_ord) {
            out << r.algoritmo << "," << r.fase << "," << r.n << ","
                << fixed << setprecision(6) << r.tempo_s << ","
                << r.comparacoes << "," << r.trocas << "\n";
        }
        cerr << "Gerado: resultados_ordenacao.csv" << endl;
    }

    // =========================================================================
    // SAÍDA — resultados_busca.csv
    // =========================================================================
    {
        ofstream out("resultados_busca.csv");
        out << "TipoBusca,Fase,N,AlvoPos,Tempo(s),Comparacoes,Encontrado\n";
        for (auto &r : resultados_busca) {
            out << r.tipo_busca << "," << r.fase << "," << r.n << ","
                << r.alvo_pos << ","
                << fixed << setprecision(9) << r.tempo_s << ","
                << r.comparacoes << ","
                << (r.indice_encontrado >= 0 ? "Sim" : "Nao") << "\n";
        }
        cerr << "Gerado: resultados_busca.csv" << endl;
    }

    return 0;
}
