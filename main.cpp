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
// TAMANHOS DE SUBCONJUNTO
// Altere este vetor para adaptar a bases maiores.
// O último valor deve ser <= total de registros do CSV.
// Exemplo para 50 000 registros: {1000, 5000, 10000, 20000, 35000, 50000}
// =========================================================================
vector<int> TAMANHOS = {50, 100, 150, 200, 250, 310};

// =========================================================================
// ESTRUTURA DE DADOS
// =========================================================================

struct Paciente {
    int pelvic_incidence; // armazenado como inteiro * 1000
    string linha_original;
};

// =========================================================================
// 1. ALGORITMOS DE ORDENAÇÃO
// =========================================================================

void bubbleSortFlag(vector<Paciente> &arr, long long &comparacoes, long long &trocas) {
    int n = arr.size();
    comparacoes = 0; trocas = 0;
    bool trocou;
    for (int i = 0; i < n - 1; i++) {
        trocou = false;
        for (int j = 0; j < n - 1 - i; j++) {
            comparacoes++;
            if (arr[j].pelvic_incidence > arr[j + 1].pelvic_incidence) {
                trocou = true;
                swap(arr[j], arr[j + 1]);
                trocas++;
            }
        }
        if (!trocou) break;
    }
}

void insertionSort(vector<Paciente> &arr, long long &comparacoes, long long &trocas) {
    int n = arr.size();
    comparacoes = 0; trocas = 0;
    for (int i = 1; i < n; i++) {
        Paciente chave = arr[i];
        int j = i - 1;
        while (j >= 0) {
            comparacoes++;
            if (arr[j].pelvic_incidence > chave.pelvic_incidence) {
                arr[j + 1] = arr[j];
                trocas++;
                j--;
            } else break;
        }
        arr[j + 1] = chave;
    }
}

void selectionSort(vector<Paciente> &arr, long long &comparacoes, long long &trocas) {
    int n = arr.size();
    comparacoes = 0; trocas = 0;
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            comparacoes++;
            if (arr[j].pelvic_incidence < arr[min_idx].pelvic_incidence)
                min_idx = j;
        }
        if (min_idx != i) { swap(arr[min_idx], arr[i]); trocas++; }
    }
}

void shellSort(vector<Paciente> &arr, long long &comparacoes, long long &trocas) {
    int n = arr.size();
    comparacoes = 0; trocas = 0;
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int j = i;
            while (j >= gap) {
                comparacoes++;
                if (arr[j - gap].pelvic_incidence > arr[j].pelvic_incidence) {
                    swap(arr[j], arr[j - gap]);
                    trocas++;
                    j -= gap;
                } else break;
            }
        }
    }
}

int particionaLomuto(vector<Paciente> &arr, int ini, int fim,
                     long long &comparacoes, long long &trocas) {
    int pivo = arr[fim].pelvic_incidence;
    int i = ini - 1;
    for (int j = ini; j < fim; j++) {
        comparacoes++;
        if (arr[j].pelvic_incidence <= pivo) { i++; swap(arr[i], arr[j]); trocas++; }
    }
    swap(arr[i + 1], arr[fim]); trocas++;
    return i + 1;
}

void quickSortLomutoRec(vector<Paciente> &arr, int ini, int fim,
                        long long &comparacoes, long long &trocas) {
    if (ini < fim) {
        int idx = particionaLomuto(arr, ini, fim, comparacoes, trocas);
        quickSortLomutoRec(arr, ini, idx - 1, comparacoes, trocas);
        quickSortLomutoRec(arr, idx + 1, fim, comparacoes, trocas);
    }
}

void quickSortLomuto(vector<Paciente> &arr, long long &comparacoes, long long &trocas) {
    comparacoes = 0; trocas = 0;
    if (!arr.empty()) quickSortLomutoRec(arr, 0, arr.size() - 1, comparacoes, trocas);
}

int particionaHoare(vector<Paciente> &arr, int ini, int fim,
                    long long &comparacoes, long long &trocas) {
    int pivo = arr[ini].pelvic_incidence;
    int i = ini - 1, j = fim + 1;
    while (true) {
        do { i++; comparacoes++; } while (arr[i].pelvic_incidence < pivo);
        do { j--; comparacoes++; } while (arr[j].pelvic_incidence > pivo);
        if (i >= j) return j;
        swap(arr[i], arr[j]); trocas++;
    }
}

void quickSortHoareRec(vector<Paciente> &arr, int ini, int fim,
                       long long &comparacoes, long long &trocas) {
    if (ini < fim) {
        int idx = particionaHoare(arr, ini, fim, comparacoes, trocas);
        quickSortHoareRec(arr, ini, idx, comparacoes, trocas);
        quickSortHoareRec(arr, idx + 1, fim, comparacoes, trocas);
    }
}

void quickSortHoare(vector<Paciente> &arr, long long &comparacoes, long long &trocas) {
    comparacoes = 0; trocas = 0;
    if (!arr.empty()) quickSortHoareRec(arr, 0, arr.size() - 1, comparacoes, trocas);
}

// -------------------------------------------------------------------------
// Merge Sort
// -------------------------------------------------------------------------

void mergeSortMerge(vector<Paciente> &arr, int esq, int meio, int dir,
                    long long &comparacoes, long long &trocas) {
    vector<Paciente> tmp(arr.begin() + esq, arr.begin() + dir + 1);
    int i = 0, j = meio - esq + 1, k = esq;
    int tam_esq = meio - esq + 1;
    while (i < tam_esq && j < (dir - esq + 1)) {
        comparacoes++;
        if (tmp[i].pelvic_incidence <= tmp[j].pelvic_incidence) {
            arr[k++] = tmp[i++];
        } else {
            arr[k++] = tmp[j++];
            trocas += tam_esq - i; // conta deslocamentos como "trocas"
        }
    }
    while (i < tam_esq)  arr[k++] = tmp[i++];
    while (j < (dir - esq + 1)) arr[k++] = tmp[j++];
}

void mergeSortRec(vector<Paciente> &arr, int esq, int dir,
                  long long &comparacoes, long long &trocas) {
    if (esq >= dir) return;
    int meio = esq + (dir - esq) / 2;
    mergeSortRec(arr, esq, meio, comparacoes, trocas);
    mergeSortRec(arr, meio + 1, dir, comparacoes, trocas);
    mergeSortMerge(arr, esq, meio, dir, comparacoes, trocas);
}

void mergeSort(vector<Paciente> &arr, long long &comparacoes, long long &trocas) {
    comparacoes = 0; trocas = 0;
    if (!arr.empty()) mergeSortRec(arr, 0, arr.size() - 1, comparacoes, trocas);
}

// -------------------------------------------------------------------------
// Radix Sort (LSD, base 10, valores inteiros >= 0)
// -------------------------------------------------------------------------

void radixSort(vector<Paciente> &arr, long long &comparacoes, long long &trocas) {
    comparacoes = 0; trocas = 0;
    if (arr.empty()) return;

    // Encontra o valor máximo para saber quantos dígitos iterar
    int maxVal = arr[0].pelvic_incidence;
    for (auto &p : arr) if (p.pelvic_incidence > maxVal) maxVal = p.pelvic_incidence;

    for (int exp = 1; maxVal / exp > 0; exp *= 10) {
        int n = arr.size();
        vector<Paciente> saida(n);
        int contagem[10] = {0};

        for (int i = 0; i < n; i++) {
            comparacoes++;                       // "leitura" do dígito como comparação
            contagem[(arr[i].pelvic_incidence / exp) % 10]++;
        }
        for (int i = 1; i < 10; i++) contagem[i] += contagem[i - 1];
        for (int i = n - 1; i >= 0; i--) {
            int digito = (arr[i].pelvic_incidence / exp) % 10;
            saida[--contagem[digito]] = arr[i];
            trocas++;
        }
        arr = saida;
    }
}

void insertionSortDesc(vector<Paciente> &arr, long long &comparacoes, long long &trocas) {
    int n = arr.size();
    comparacoes = 0; trocas = 0;
    for (int i = 1; i < n; i++) {
        Paciente chave = arr[i];
        int j = i - 1;
        while (j >= 0) {
            comparacoes++;
            if (arr[j].pelvic_incidence < chave.pelvic_incidence) {
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
void heapify(vector<Paciente> &arr, int n, int i,
             long long &comparacoes, long long &trocas) {
    int maior = i;
    int esq   = 2 * i + 1;
    int dir   = 2 * i + 2;

    if (esq < n) {
        comparacoes++;
        if (arr[esq].pelvic_incidence > arr[maior].pelvic_incidence)
            maior = esq;
    }

    if (dir < n) {
        comparacoes++;
        if (arr[dir].pelvic_incidence > arr[maior].pelvic_incidence)
            maior = dir;
    }

    if (maior != i) {
        swap(arr[i], arr[maior]);
        trocas++;
        heapify(arr, n, maior, comparacoes, trocas); // afunda recursivamente
    }
}

void heapSort(vector<Paciente> &arr, long long &comparacoes, long long &trocas) {
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
// 3. ALGORITMOS DE BUSCA
// =========================================================================

int buscaSequencial(const vector<Paciente> &arr, int alvo, long long &comparacoes) {
    comparacoes = 0;
    for (int i = 0; i < (int)arr.size(); i++) {
        comparacoes++;
        if (arr[i].pelvic_incidence == alvo) return i;
    }
    return -1;
}

int buscaBinaria(const vector<Paciente> &arr, int alvo, long long &comparacoes) {
    comparacoes = 0;
    int esq = 0, dir = arr.size() - 1;
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;
        comparacoes++;
        if (arr[meio].pelvic_incidence == alvo) return meio;
        else if (arr[meio].pelvic_incidence < alvo) esq = meio + 1;
        else dir = meio - 1;
    }
    return -1;
}

// =========================================================================
// 3. LEITURA DO CSV
// =========================================================================

vector<Paciente> lerCSV(const string &caminho, string &cabecalho) {
    vector<Paciente> dados;
    ifstream arquivo(caminho);
    if (!arquivo.is_open()) { cerr << "Erro: não foi possível abrir " << caminho << endl; exit(1); }
    getline(arquivo, cabecalho);
    string linha;
    while (getline(arquivo, linha)) {
        if (linha.empty()) continue;
        istringstream ss(linha);
        string campo;
        getline(ss, campo, ',');
        try {
            Paciente p;
            p.pelvic_incidence = (int)(stod(campo) * 1000.0);
            p.linha_original = linha;
            dados.push_back(p);
        } catch (...) {}
    }
    return dados;
}

// =========================================================================
// 4. ESTRUTURAS DE SAÍDA
// =========================================================================

struct Algoritmo {
    string nome;
    function<void(vector<Paciente>&, long long&, long long&)> func;
};

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
    string caminho_csv = (argc > 1) ? argv[1] : "Dataset_spine.csv";
    string cabecalho;

    vector<Paciente> dados_todos = lerCSV(caminho_csv, cabecalho);
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
        vector<Paciente> sub(dados_todos.begin(), dados_todos.begin() + tam);

        // Alvos: índice 0, tam/2, tam-1 (posição relativa ao subconjunto)
        // Guardamos a posição (0, 1, 2) para facilitar rótulos no Python
        vector<pair<int,int>> alvos = {
            {0, sub[0].pelvic_incidence},
            {1, sub[tam / 2].pelvic_incidence},
            {2, sub[tam - 1].pelvic_incidence}
        };

        // --- Fase A: desordenado → busca sequencial ---
        for (auto &[pos, alvo] : alvos) {
            long long c = 0;
            clock_t ini = clock();
            int idx = buscaSequencial(sub, alvo, c);
            clock_t fim = clock();
            resultados_busca.push_back({"Sequencial", "Desordenado", tam, pos,
                                        (double)(fim - ini) / CLOCKS_PER_SEC, c, idx});
        }

        // --- Fase B: desordenado → ordenação ---
        vector<Paciente> sub_ord_asc;
        for (auto &algo : algoritmos) {
            vector<Paciente> copia = sub;
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
            vector<Paciente> copia = sub_ord_asc;
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
        }

        // --- Fase E: ordenar decrescente ---
        vector<Paciente> sub_desc = sub_ord_asc;
        {
            long long c = 0, t = 0;
            clock_t ini = clock();
            insertionSortDesc(sub_desc, c, t);
            clock_t fim = clock();
            resultados_ord.push_back({"InsertionDesc", "Crescente->Decrescente", tam,
                                      (double)(fim - ini) / CLOCKS_PER_SEC, c, t});
        }

        // --- Fase F: busca sequencial em dados decrescentes ---
        for (auto &[pos, alvo] : alvos) {
            long long c = 0;
            clock_t ini = clock();
            int idx = buscaSequencial(sub_desc, alvo, c);
            clock_t fim = clock();
            resultados_busca.push_back({"Sequencial", "Ordenado_Decrescente", tam, pos,
                                        (double)(fim - ini) / CLOCKS_PER_SEC, c, idx});
        }

        // --- Fase G: decrescente → ordenação crescente ---
        for (auto &algo : algoritmos) {
            vector<Paciente> copia = sub_desc;
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
