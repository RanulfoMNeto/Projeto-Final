# Macro-entrega 2

## Descrição e Instruções

A resolucão do projeto fundamentou-se no método não supervisionado de segregação *K-Medoids*, proposto em 1987 por Kaufman e Rousseeuw, semelhante à técnica de agrupamento *K-Means*, no entanto, difere muito em termos da maneira como seleciona os centros dos clusters. O último seleciona a média dos pontos de um cluster como seu centro (que pode ou não ser um dos pontos de dados), enquanto o primeiro sempre escolhe os pontos de dados reais dos clusters como seus centros (também conhecidos como 'exemplares' ou 'medóides'). *K-Medoides* também difere a esse respeito do algoritmo *K-Medians* que é o mesmo que *K-means*, exceto que ele escolhe as medianas (em vez de médias) dos clusters como centros.
Em ambos os casos, é necessário determinar previamente o número de clusters do conjunto de dados.

O método do cotovelo ("*Elbow Method*") é uma heurística de representação gráfica de encontrar o '*K*' ideal em um agrupamento de *K-Means*. Funciona encontrando *WCSS* (In-Cluster Sum of Square), ou seja, a soma da distância quadrada entre os pontos em um cluster e o centróide do cluster.

![Distortion Score Elbow](https://github.com/RanulfoMNeto/Projeto-Final/blob/main/instances/Distortion%20Score%20Elbow/bar-n100-1.png "Distortion Score Elbow: bar-n100-1")

Instance: *bar-n100-1*

Nesse repositório será possível encontrar a pasta “*instances/*”, contendo internamente um conjunto de imagens (em “Distortion Score Elbow/”) criadas utilizando a biblioteca Yellowbrick com o seguinte código:

```python
import pandas as pd
from sklearn.cluster import KMeans
from yellowbrick.cluster import KElbowVisualizer
from sklearn.preprocessing import StandardScaler

file_name = 'instance_name.csv'

data = pd.read_csv(file_name)

delivery = data.loc[data["dem"] < 0]

number = len(delivery)/2

clusters = 100

if (number < 100):
  clusters = number

# Instantiate the clustering model and visualizer
model = KMeans()
visualizer = KElbowVisualizer(model, k=(1,int(clusters)))
X = delivery[['lat','lon']]

scaler = StandardScaler()
data_scaled = scaler.fit_transform(X)

visualizer.fit(data_scaled) # Fit the data to the visualizer
visualizer.show(outpath="distortion/"+file_name+".png") # Finalize and render the figure
```

Todos os resultados obtidos através desse método adequaram-se ao algoritmo de resolução considerando os respectivos números de clusters gerados. Também foi constatado que ao utilizar o método de clusterização, o algoritmo de solução produz em média um resultado de melhora superior à 1%, em relação à saída sem a utilização da técnica.

![Comparison](https://github.com/RanulfoMNeto/Projeto-Final/blob/main/comparison.png "Comparison")

O arquivo "*benchmarks.txt*" é responsável pela entrada de instâncias e seus respectivos número de cluster, nele é possível alterar os valores e verificar a execução sem a técnica de clusterização ("clusters = 1").

Obs: Os valores incorporados ao arquivo são informações padrões
- `name`: Nome da instância;
- `M`: Número de veículos gastos pela melhor solução conhecida;
- `C`: Custo gasto pelos veículos da melhor solução conhecida;
- `MX`: Número de veículos gastos pela solução gerada;
- `CX`: Custo gasto pelos veículos da solução gerada;
- `clusters`: Número de clusters da solução gerada.

e devem sempre que possível não serem modificadas (com exceção da coluna “*clusters*”), com o objetivo de não influenciar a execução e integridade da solução final.
