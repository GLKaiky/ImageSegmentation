import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit

# 1. Carregar dados
data = pd.read_csv('benchmark_data.csv')
x = data['pixels'] # Tamanho da entrada (N)
y = data['time_seconds'] # Tempo (t)

# 2. Definir funções de complexidade para comparar
def linear(x, a, b):
    return a * x + b

def n_log_n(x, a, b):
    return a * x * np.log(x) + b

def quadratic(x, a, b):
    return a * x**2 + b

# 3. Plotar os dados reais
plt.figure(figsize=(10, 6))
plt.scatter(x, y, color='red', label='Seu Código (Dados Reais)', zorder=5)
plt.plot(x, y, color='red', linestyle='--', alpha=0.5)

# 4. Tentar encaixar curvas teóricas (Regressão)
try:
    popt_quad, _ = curve_fit(quadratic, x, y)
    plt.plot(x, quadratic(x, *popt_quad), label='Curva Quadrática ($O(N^2)$)', linestyle='-')
    
    popt_nlogn, _ = curve_fit(n_log_n, x, y)
    plt.plot(x, n_log_n(x, *popt_nlogn), label='Curva Linearítmica ($O(N \log N)$)', linestyle=':')
except:
    print("Dados insuficientes para fitting perfeito, plotando apenas os pontos.")

# 5. Estética do Gráfico
plt.title('Análise de Complexidade: Segmentação de Imagem')
plt.xlabel('Tamanho da Entrada (Número de Pixels)')
plt.ylabel('Tempo de Execução (Segundos)')
plt.legend()
plt.grid(True, which="both", ls="-")
plt.show()