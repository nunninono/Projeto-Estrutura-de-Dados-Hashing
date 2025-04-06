****O que é Hashing?****
*Hashing é uma técnica utilizada para armazenar, buscar e acessar dados de forma rápida. Através de uma função hash, uma chave (como uma string ou número) é convertida em um índice de uma tabela. Essa abordagem permite operações eficientes de inserção, busca e remoção, geralmente em tempo constante*



**Projeto de Cadastro de Carros com Hashing em C**

  -Este projeto implementa um sistema de cadastro de carros com acesso direto via hashing, utilizando arquivos binários e listas duplamente encadeadas para indexação.



**Estrutura e Funcionamento:**

  -Os carros são armazenados em um arquivo binário (carros.dat) com campos como placa, marca, modelo, cor e status (ativo/removido).

  -A busca é feita através de uma tabela hash de tamanho fixo (TAM = 57), com listas encadeadas para tratar colisões (encadeamento externo).

  -Cada carro é indexado na tabela pela placa, convertida em um índice através de uma função hashing().




**Funcionalidades:**

  -Cadastrar carro: Salva no arquivo e adiciona na tabela hash.

  -Consultar carro: Busca pela placa e mostra os dados.

  -Alterar carro: Permite editar marca, modelo ou cor.

  -Remover carro: Marca o status como removido (remoção lógica) e remove da tabela hash.

  -Exibir carros ativos: Lista todos os carros com status = 1.

  -Encerrar programa: Limpa o arquivo, removendo registros com status = 0, e desaloca a tabela hash.




**Técnicas e Estruturas Utilizadas**

  -Hashing com tratamento de colisões

  -Listas duplamente encadeadas

  -Manipulação de arquivos binários

  -Remoção lógica + compactação do arquivo

  -Função hash baseada em deslocamento de bits
