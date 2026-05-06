@echo off
REM Script para executar graficos.py com Python 3.12
REM Este script verifica se as dependências estão instaladas antes de executar

echo Executando graficos.py com Python 3.12...
echo.

python3.12.exe graficos.py

if %errorlevel% equ 0 (
    echo.
    echo Sucesso! Os graficos foram gerados na pasta Graficos\
) else (
    echo.
    echo Erro ao executar o script!
    echo Instalando dependencias...
    python3.12.exe -m pip install -r requirements.txt
    echo.
    echo Tentando novamente...
    python3.12.exe graficos.py
)

pause
