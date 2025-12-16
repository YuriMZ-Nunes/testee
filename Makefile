CXX      = g++
CXXFLAGS = -O3 -march=native -Wall -std=c++17

OMPFLAGS = -fopenmp

# Flag opcional: Descomente abaixo para ver o relatório do compilador sobre vetorização (útil para debug da Tarefa C)
# CXXFLAGS += -fopt-info-vec-optimized

SRC_DIR = src
SEQ_DIR = $(SRC_DIR)/seq
OMP_DIR = $(SRC_DIR)/omp
BIN_DIR = bin
RES_DIR = results

TARGET_SEQ_C = $(BIN_DIR)/task_c_seq
TARGET_OMP_C = $(BIN_DIR)/task_c_omp
TARGET_OMP_D = $(BIN_DIR)/task_d_omp


.PHONY: all directories seq omp run plot clean help

all: directories seq omp

directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(RES_DIR)

seq: $(TARGET_SEQ_C)

$(TARGET_SEQ_C): $(SEQ_DIR)/main_seq.cpp
	@echo "Compilando Sequencial (Tarefa C)..."
	$(CXX) $(CXXFLAGS) -o $@ $<

omp: $(TARGET_OMP_C) $(TARGET_OMP_D)

$(TARGET_OMP_C): $(OMP_DIR)/main_c_omp.cpp
	@echo "Compilando OpenMP (Tarefa C - SIMD)..."
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) -o $@ $<

$(TARGET_OMP_D): $(OMP_DIR)/main_d_omp.cpp
	@echo "Compilando OpenMP (Tarefa D - Overhead)..."
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) -o $@ $<

run: all
	@echo "Iniciando bateria de testes..."
	@chmod +x run.sh
	./run.sh

plot: $(RES_DIR)/dados.csv
	@echo "Gerando gráficos..."
	python3 plot.py

clean:
	@echo "Limpando binários e artefatos..."
	rm -rf $(BIN_DIR)

help:
	@echo "Opções do Makefile:"
	@echo "  make all   - Compila tudo (sequencial e omp)"
	@echo "  make seq   - Compila apenas binários sequenciais"
	@echo "  make omp   - Compila apenas binários OpenMP"
	@echo "  make run   - Compila e roda o script de testes (run.sh)"
	@echo "  make plot  - Gera gráficos a partir dos resultados"
	@echo "  make clean - Remove a pasta bin/"