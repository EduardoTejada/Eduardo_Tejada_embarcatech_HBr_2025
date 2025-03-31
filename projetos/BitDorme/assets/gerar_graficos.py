import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime

# Função para ler o arquivo e extrair os dados
def ler_arquivo(nome_arquivo):
    horarios = []
    valores = []

    with open(nome_arquivo, 'r') as file:
        for linha in file:
            linha = linha.strip()  # Remove espaços em branco e quebras de linha
            if linha:
                linha = linha.replace(' ', '')  # Remove todos os espaços
                # Divide a linha no formato "HH:MM:SS - NN"
                partes = linha.split('-')
                if len(partes) == 2:
                    horario_str = partes[0]  # Extrai o horário (HH:MM:SS)
                    valor_str = partes[1]   # Extrai o valor (NN)

                    # Converte o horário para um objeto datetime
                    horario = datetime.strptime(horario_str, '%H:%M:%S')
                    # Converte o valor para um número inteiro ou float
                    valor = float(valor_str)

                    # Adiciona aos arrays
                    horarios.append(horario)
                    valores.append(valor)

    return horarios, valores

# Função para plotar o gráfico
def plotar_grafico(horarios, valores):
    plt.figure(figsize=(10, 6))  # Define o tamanho da figura

    # Plota o gráfico de linha
    plt.plot(horarios, valores, marker='o', linestyle='-', color='b', label='Nível')

    # Formata o eixo X para exibir horários
    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M:%S'))
    plt.gca().xaxis.set_major_locator(mdates.SecondLocator(interval=1))  # Intervalo de 1 segundo
    plt.gcf().autofmt_xdate()  # Rotaciona os rótulos do eixo X para melhor legibilidade

    # Adiciona títulos e rótulos
    plt.title("Gráfico do Nível de volume ao Longo do Tempo")
    plt.xlabel("Horário")
    plt.ylabel("Volume")
    plt.grid(True)  # Adiciona uma grade ao gráfico
    plt.legend()  # Adiciona uma legenda

    plt.savefig('grafico.png')

    # Exibe o gráfico
    plt.show()

# Nome do arquivo de entrada
nome_arquivo = 'dados.txt'

# Lê os dados do arquivo
horarios, valores = ler_arquivo(nome_arquivo)

# Plota o gráfico
plotar_grafico(horarios, valores)