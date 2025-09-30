# Nome do arquivo: contador_de_segmentos.py

from PIL import Image
import sys

def contar_segmentos_por_cor(caminho_da_imagem):
    """
    Conta o número de segmentos em uma imagem segmentada,
    assumindo que cada segmento é representado por uma cor única.

    Args:
        caminho_da_imagem (str): O caminho para o arquivo de imagem.

    Returns:
        int: O número de cores únicas (segmentos) encontradas, ou None se ocorrer um erro.
    """
    try:
        # Abre a imagem
        img = Image.open(caminho_da_imagem)

        # 'getdata()' retorna uma sequência com os dados de cada pixel.
        # Criamos um 'set' (conjunto) a partir desses dados.
        # Um 'set' só armazena valores únicos, eliminando duplicatas automaticamente.
        cores_unicas = set(img.getdata())

        # O número de segmentos é o tamanho do conjunto de cores únicas.
        return len(cores_unicas)

    except FileNotFoundError:
        print(f"Erro: O arquivo de imagem '{caminho_da_imagem}' não foi encontrado.")
        return None
    except Exception as e:
        print(f"Ocorreu um erro ao processar a imagem: {e}")
        return None

# --- Como usar o script ---
if __name__ == "__main__":
    # Pega o caminho da imagem a partir do argumento da linha de comando
    # Exemplo de como rodar: python contador_de_segmentos.py minha_imagem.png
    if len(sys.argv) > 1:
        caminho_imagem = sys.argv[1]
    else:
        # Se nenhum caminho for fornecido, usa um exemplo.
        # Altere o nome do arquivo aqui para a imagem que você quer analisar.
        caminho_imagem = "resultado_segmentado4.png" 
        print(f"Aviso: Nenhum caminho de imagem fornecido. Usando o exemplo: '{caminho_imagem}'")


    print(f"\nAnalisando a imagem: {caminho_imagem}")
    
    numero_de_segmentos = contar_segmentos_por_cor(caminho_imagem)

    if numero_de_segmentos is not None:
        print("-------------------------------------------")
        print(f"Resultado: A imagem possui {numero_de_segmentos} segmentos.")
        print("-------------------------------------------")