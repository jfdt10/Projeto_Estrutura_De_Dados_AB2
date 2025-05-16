# Carrega as bibliotecas necessárias para a plotagem e manipulação de dados.
# ggplot2 é para criar gráficos.
# dplyr é para manipulação de dados (como group_by, summarise).
# tidyr é para "arrumar" os dados (como pivot_longer).
library(ggplot2)
library(dplyr)
library(tidyr)

# Define o nome do arquivo CSV de onde os dados serão lidos.
arquivo_dados <- "dados_busca.csv"
# Define o nome do arquivo de imagem onde o gráfico será salvo.
arquivo_grafico <- "grafico_busca_media.png" 

# Lê os dados do arquivo CSV para um dataframe chamado dados_completos.
dados_completos <- read.csv(arquivo_dados)

# Transforma os dados do formato "largo" para o formato "longo".
# Isso é útil para o ggplot2, pois facilita a plotagem de múltiplas séries.
# As colunas ComparacoesLista e ComparacoesBST são "empilhadas" em duas novas colunas:
# EstruturaBruta (contendo os nomes das colunas originais) e Comparacoes (contendo os valores).
dados_long <- dados_completos %>%
  pivot_longer(cols = c(ComparacoesLista, ComparacoesBST),
               names_to = "EstruturaBruta",
               values_to = "Comparacoes") %>%
  # Cria uma nova coluna 'Estrutura' com nomes mais descritivos para as legendas do gráfico.
  mutate(Estrutura = ifelse(EstruturaBruta == "ComparacoesLista", "Lista Encadeada", "Árvore BST"))

# Calcula a média de comparações para cada NumeroProcurado e para cada Estrutura.
# Agrupa os dados por NumeroProcurado e Estrutura.
dados_media <- dados_long %>%
  group_by(NumeroProcurado, Estrutura) %>%
  # Calcula a média da coluna 'Comparacoes', ignorando valores NA (na.rm = TRUE).
  summarise(ComparacoesMedias = mean(Comparacoes, na.rm = TRUE)) %>%
  # Desagrupa os dados para operações futuras.
  ungroup() %>%
  # Ordena o dataframe resultante pela coluna NumeroProcurado, para que o gráfico tenha o eixo x ordenado.
  arrange(NumeroProcurado) 

# Define um vetor de cores nomeado para usar no gráfico.
# Isso garante que a Lista Encadeada sempre tenha a cor azul e a Árvore BST a cor vermelha.
cores_linhas <- c("Lista Encadeada" = "deepskyblue3", "Árvore BST" = "firebrick2")

# Cria o objeto do gráfico usando ggplot.
# dados_media é o dataframe a ser usado.
# aes define a estética: NumeroProcurado no eixo x, ComparacoesMedias no eixo y.
# color e group são mapeados para a coluna Estrutura, para desenhar linhas separadas e coloridas para cada estrutura.
grafico <- ggplot(dados_media, aes(x = NumeroProcurado, y = ComparacoesMedias, color = Estrutura, group = Estrutura)) +
  # Adiciona uma camada de linha suavizada (geom_smooth).
  # method = "gam" usa um Modelo Aditivo Generalizado para a suavização.
  # formula = y ~ s(x, k = 15, bs = "cs") especifica como a suavização é calculada (spline cúbico com k nós).
  # se = FALSE remove a banda de erro padrão (intervalo de confiança).
  # linewidth define a espessura da linha.
  geom_smooth(method = "gam", formula = y ~ s(x, k = 15, bs = "cs"), se = FALSE, linewidth = 0.8) +
  # Define manualmente as cores a serem usadas para as linhas, com base no vetor 'cores_linhas'.
  scale_color_manual(values = cores_linhas) +
  # Cria painéis (facetas) separados para cada Estrutura.
  # scales = "free_y" permite que cada faceta tenha sua própria escala no eixo y.
  # ncol = 2 organiza as facetas em 2 colunas.
  facet_wrap(~ Estrutura, scales = "free_y", ncol = 2) + 
  # Define os rótulos (título, subtítulo, eixos) do gráfico.
  labs(
    title = "Média de Comparações por Número Procurado (Ordenado)",
    subtitle = paste("Dados de:", arquivo_dados, "| Estruturas com 10000 elementos | Execução única"),
    x = "Número Procurado (Ordenado)",
    y = "Número Médio de Comparações",
    color = "Estrutura de Dados" # Rótulo da legenda de cores.
  ) +
  # Aplica um tema visual mais claro ao gráfico (theme_light).
  # base_size define o tamanho base da fonte.
  theme_light(
    base_size = 12
  ) +
  # Personaliza elementos específicos do tema.
  theme(
    plot.title = element_text(hjust = 0.5, face = "bold"), # Centraliza e negrita o título.
    plot.subtitle = element_text(hjust = 0.5, size = 10), # Centraliza e define o tamanho do subtítulo.
    legend.position = "top", # Posiciona a legenda no topo.
    axis.text.x = element_text(angle = 45, hjust = 1) # Rotaciona os rótulos do eixo x para melhor visualização.
  )

# Salva o gráfico gerado em um arquivo PNG.
# filename especifica o nome do arquivo.
# plot é o objeto do gráfico a ser salvo.
# width e height definem as dimensões do gráfico em polegadas.
# dpi define a resolução (pontos por polegada).
ggsave(filename = arquivo_grafico, plot = grafico, width = 12, height = 7, dpi = 300)

# Imprime uma mensagem no console confirmando que o gráfico foi salvo.
print(paste("Gráfico de médias salvo como:", arquivo_grafico))
