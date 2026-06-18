INSTRUÇÕES SOBRE COMO RODAR OS EXP
  
* importante dar um make caso não tenha dado, ele gera automáticamente todos os executáveis dos 4 experimentos
* todos os executáveis estão na pasta /bin então entrar nessa pasta primeiro

=====================================================


-> experimento 1: a ideia é comparar o desempenho dos miss rate entre cada modo (single, uncorrelated, anticorrelated, correlated), especialmente correlated e uncorrelated. Assim:

./exp1_global_history [modo]

ex: 

./exp1_global_history single
./exp1_global_history uncorrelated
./exp1_global_history anticorrelated
./exp1_global_history correlated

(!) gerar tabela simples, com modo e miss_rate



=======================================================


-> experimento 2: a ideia é testar cada modo com gaps diferentes (0, 1, 16, 32, ..., etc), em outras palavras, até que ponto consegue lembrar da branch anterior em cada modo. Assim:

./exp2_global_distance [modo] [gap]

ex:

./exp2_global_distance correlated 1
./exp2_global_distance anticorrelated 1
./exp2_global_distance uncorrelated 1

./exp2_global_distance correlated 16
./exp2_global_distance anticorrelated 16
./exp2_global_distance uncorrelated 16

(!) gerar tabela simples com modo, gap e miss_rate


========================================================

-> experimento 3: a ideia é verificar os dois modos <local, random> com periodos e gaps diferentes. 

Para modo <local> precisa passar o periodo, um valor OBRIGATORIAMENTE Potencia de 2 e gap. Assim:

./exp3_local_history local [periodo] [gap]

Para modo <random> só precisa passar o gap. Assim:

./exp3_local_history random [gap]

Ex:

./exp3_local_history local 4 0
./exp3_local_history random 0

./exp3_local_history local 4 8 
./exp3_local_history random 8

./exp3_local_history local 32 32
./exp3_local_history random 32

(!) importante comparar com os dois modos tendo o mesmo gap para fazer sentido


==========================================================

experimento 4: Esse experimento tem dois tipos <global, local>, a ideia é testar cada tipo com valores diferentes e incrementais até determinar onde o preditor começa a "falhar", isso indica uma estimativa do possível tamanho. 

Este experimento é um compilado dos experimentos 2 e 3 simplificado, então os modos internos são semelhantes a aqueles exp anteriores.

Então para o tipo <global> usa assim:

./exp4_effective_size global [modo] [gap]

onde [modo] pode ser <correlated|uncorrelated|anticorrelated>

e [gap] pode ser qualquer valor <0, 1,2, 4, 8, ...>

Ex: 

./bin/exp4_effective_size global correlated 0
./bin/exp4_effective_size global uncorrelated 0
./bin/exp4_effective_size global anticorrelated 0

./bin/exp4_effective_size global correlated 1
./bin/exp4_effective_size global uncorrelated 1
./bin/exp4_effective_size global anticorrelated 1

./bin/exp4_effective_size global correlated 4
./bin/exp4_effective_size global uncorrelated 4
./bin/exp4_effective_size global anticorrelated 4

(!) quando criar gráficos, colocar gap, miss_rate para cada modo e adicionar delta:

delta(gap) = miss_rate_uncorrelated(gap) - miss_rate_correlated(gap)

Se delta é positivo e relevante, o correlacionado está melhor.


Para o tipo <local> usa assim:

+ local que precisa periodo e gap: 

./exp4_effective_size global local [periodo] [gap]

onde [periodo] obrigoriamente é qualquer valor potencia de 2
onde [gap] pode ser qualquer valor <0,1,2, ...>

+ random que só precisa de gap:
+ 
./exp4_effective_size random [gap]

Ex: 

./bin/exp4_effective_size local 2 0
./bin/exp4_effective_size local 4 0
./bin/exp4_effective_size local 8 0
./bin/exp4_effective_size local 16 0
./bin/exp4_effective_size local 32 0
./bin/exp4_effective_size local 64 0
./bin/exp4_effective_size random 0

./bin/exp4_effective_size local 2 32
./bin/exp4_effective_size local 4 32
./bin/exp4_effective_size local 8 32
./bin/exp4_effective_size local 16 32
./bin/exp4_effective_size local 32 32
./bin/exp4_effective_size local 64 32
./bin/exp4_effective_size random 32

(!) quando criar gráficos colocar perioro, gap, local miss rate, random miss rate

criar gráficos com miss_rate = branch-misses / branches
cycles_per_branch = cycles / branches



