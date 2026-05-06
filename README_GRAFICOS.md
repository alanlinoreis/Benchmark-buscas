# Solução para o erro do graficos.py

## Problema
O script `graficos.py` não estava funcionando porque:
1. As dependências Python (`pandas`, `matplotlib`, `numpy`) não estavam instaladas
2. A versão Python 3.14.3 no venv não tinha suporte a rodas pré-compiladas para esses pacotes

## Solução Aplicada
As dependências foram instaladas usando **Python 3.12** que possui suporte completo aos pacotes necessários.

## Como usar

### Opção 1: Executar via script batch (Windows)
```
executar_graficos.bat
```

### Opção 2: Executar via terminal PowerShell
```powershell
python3.12.exe graficos.py
```

### Opção 3: Instalar dependências e usar Python 3.14 (se compilador C++ estiver disponível)
```powershell
python3.12.exe -m pip install -r requirements.txt
```

## Dependências Instaladas
- **pandas**: Leitura e processamento de dados CSV
- **matplotlib**: Geração de gráficos
- **numpy**: Operações matemáticas

## Gráficos Gerados
O script gera os seguintes gráficos na pasta `Graficos/`:
- `ord_comparacoes.png` - Comparações de ordenação por fase
- `ord_trocas.png` - Trocas de ordenação por fase
- `ord_tempos.png` - Tempo de execução de ordenação
- `busca_comparacoes.png` - Comparações de busca
- `busca_tempo.png` - Tempo de busca
- `resumo_ordenacao.png` - Resumo de ordenação por algoritmo

## Próximas Executions
Para executar o script novamente, basta usar:
```powershell
python3.12.exe graficos.py
```
