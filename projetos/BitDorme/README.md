# BitDorme

## Escopo do projeto
1. Apresentação do projeto – breve descrição sobre o que trata o projeto.  
O projeto consiste em um sistema de monitoramento do sono utilizando o microfone da BitDogLab, aliado a um alarme interativo que inclui um puzzle para facilitar o despertar de forma mais eficiente.
 
2. Título do projeto – pequeno título para o projeto.  
“BitDorme: Monitoramento de sono e despertador inteligente”
  
3. Objetivos do projeto – descreva os objetivos do projeto.  
Busca-se fazer um sistema capaz de coletar dados de áudio durante a noite para que assim seja possível identificar padrões e ajudar a melhorar a qualidade do sono dos usuários
Também se deseja criar um jeito de fazer com que o usuário resolva um puzzle para fazer com que o alarme pare de tocar. Assim estimulando a atividade cognitiva fazendo com que não seja tão simples desligar o alarme e voltar a dormir.
Com a combinação de ambas tarefas o projeto contribui para uma rotina matinal mais produtiva e uma qualidade de sono melhor.
  
5. Principais requisitos – podem ser estipulados pelo usuário ou cliente, ou até por você (fazendo o papel de um deles).
  1. Requisitos funcionais:  
  - Monitoramento de áudio: O sistema deve captar sons do ambiente, como respiração e ronco, para análise da qualidade do sono.
  - Armazenamento de dados: Deve registrar informações relevantes durante a noite para posterior análise.
  - Ativação do alarme: O despertador deve ser acionado em um horário pré-definido.
  - Desbloqueio do alarme por puzzle: Para desligar o alarme, o usuário deve resolver um desafio (exemplo: sequência lógica, cálculo matemático, jogo simples).
  - Interface de usuário: Deve ter uma forma de configuração e visualização de dados.
  
  2. Requisitos não funcionais:  
  - Custo acessível: O projeto deve buscar componentes de baixo custo para viabilizar sua produção.
  - Baixa latência no processamento: A análise de áudio deve ser feita em tempo real ou com um pequeno atraso para garantir a precisão.
  - Durabilidade: Os componentes devem ser resistentes o suficiente para suportar o uso diário sem falhas.

5. Descrição do funcionamento – descreva as funcionalidades do projeto.  
Os sons captados são processados em tempo real e armazenados para futura análise, possibilitando que o usuário visualize informações relevantes sobre seu sono.
Há uma interface de configuração, permitindo ao usuário configurar o horário do despertador.
Para garantir que o usuário realmente acorde, o alarme só pode ser desligado após a resolução de um jogo da memória, estimulando a atividade mental e reduzindo as chances de voltar a dormir.
É mostrado no display OLED dos resultados coletados durante o sono, bem como é disponibilizado um código em Python capaz de gerar um na tela do computador para melhor visualização.

6. Justificativa – mostre que a execução do projeto se justifica.  
O sono é fundamental para a vida de todos, e garantir que ele esteja sendo realizado com qualidade é uma tarefa importante. Durante o sono o corpo realiza diversos processos essenciais e distúrbios no sono podem causar diversos problemas não só em relação ao comportamento mas também podem levar ao desenvolvimento de doenças crônicas.
Por outro lado, despertar de um sono profundo nem sempre é uma tarefa fácil, muitas pessoas, mesmo que sem querer, desligam o alarme e voltam a dormir sem nem perceber. Para combater isso, diversos aplicativos usam desafios para estimular o pensamento nos primeiros segundos do dia, assim ajudando a trazer o cérebro para a consciência mais rápido. Esses podem ser problemas matemáticos ou desafios de lógica, mas todos buscam minimizar a sonolência matinal e melhorar a disposição ao longo do dia.
Por conta disso o uso de um dispositivo que não só ajuda a monitorar o sono, coletando dados que podem indicar diversos problemas mas também que auxilia o usuário a acordar tende a ser muito benéfico para a saúde, rotina e bem-estar.

7. Originalidade – mostre através de uma pesquisa que existem projetos correlatos, mas não iguais.  
 - [Sleep Quality Monitor](https://projecthub.arduino.cc/SrVassili/sleep-quality-monitor-48a511): Um projeto semelhante que utiliza de mais sensores, permitindo conclusões mais precisas para a análise, mas não permite que os dados sejam salvos, visualizados através do computador e fazendo uma análise mais complexa. Além de utilizar ATMEGA328P, o que limita o poder de processamento do projeto. Ainda, não há a parte do despertador;
 - [A wearable wristband designed for sleep monitoring and analysis](https://github.com/NicolasHu11/Arduino-Sleep-Monitor): Esse projeto trás uma proposta interessante de uma pulseira que para fazer a detecção dos parâmetros, também dispõe de mais sensores e é feito com ATMEGA328P como no projeto anterior. O projeto também se torna um pouco diferente por se tratar não só de um detector para o sono, mas sim para ser usado 24 horas. Ainda, não há a parte do despertador;
 - [I made an alarm clock that you can't turn off from bed](https://www.reddit.com/r/arduino/comments/c2x9ww/i_made_an_alarm_clock_that_you_cant_turn_off_from/): Como não foi encontrado nenhum projeto de sistema embarcado de um despertador que para de tocar através de um puzzle, esse é algo semelhante, mas ao invés de um jogo da memória ele obriga o usuário a levantar da cama para desligar, porém não contém a parte de monitoramento de sono.

## Hardware

1. Diagrama em blocos – diagrama mostrando os blocos e sua interligação.
![Diagrama de Blocos](https://github.com/EduardoTejada/BitDorme/blob/main/img/diagrama.drawio.png?raw=true)  Figura 1: Diagrama em Blocos
2. Função de cada bloco – descreva a função que cada bloco terá no projeto.
 - Microcontrolador: Responsável pelo gerenciamento central do sistema, o microcontrolador executa o código programado, coordena a comunicação entre os componentes, processa os dados captados pelo microfone e sensores, além de armazenar e transmitir informações relevantes para o funcionamento do dispositivo;
 - RTC: Recurso interno que conta o tempo do despertador;
 - Flash: Recurso interno que armazena os dados para análise futura;
 - Joystick: Utilizado para navegação no menu e como recurso principal no jogo da memória, através do ADC;
 - Botões: Utilizado principalmente para navegação e para parar o alarme, lidos utilizando gpio e interrupção;
 - Microfone: Principal sensor responsável por captar nível sonoro que será guardado e servirá para análises futuras, lido utilizando DMA e ADC;
 - Matriz de LED’s: O jogo da memória é jogado diretamente na matriz de LED’s, ela também gera um feedback luminoso do despertador, acionados através do PIO;
 - Buzzer: Toca o som do alarme do despertador, essencial para a aplicação, acionado através de PWM;
 - Display OLED: Responsável por dar instruções, exibir o menu de navegação e exibir gráficos, acionado através de I2C.
4. Especificações – descreva como as especificações técnicas atendem os requisitos  do cliente ou usuário.
 1. Requisitos funcionais:
 - Monitoramento de áudio: Atendido pelo Microfone, capaz de capturar sons do ambiente, incluindo respiração e ronco;
 - Armazenamento de dados: Atendido pela Memória Flash, os dados do áudio são armazenados em uma alocação dinâmica durante o tempo de execução do código e também na Flash interna do microcontrolador, permitindo posterior análise;
 - Ativação do alarme: Atendido pelo RTC, Buzzer e Matriz de LEDs, o RTC ativa o alarme no momento programado, o buzzer gera o som do alarme e a Matriz de LEDs fornece feedback visual sincronizados com o alarme. É importante notar, contudo, que o RTC inteiro da RP2040 tem uma determinada imprecisão;
 - Desbloqueio do alarme por puzzle: atendido pelo Joystick, botões e Matriz de LEDs, o Joystick e os é usado para interagir com o desafio do alarme e a matriz de LEDs exibe o jogo da memória;
 - Interface de usuário: atendido pelo display OLED, Joystick e Botões, o display OLED exibe menus e instruções para configuração do alarme, o joystick e os botões permitem a navegação no menu.
 2. Requisitos não funcionais:
 - Custo acessível: Atendido pela escolha dos componentes, RP2040, display OLED, Matriz de LEDs e o Buzzer são componentes de baixo custo;
 - Baixa latência no processamento: atendido pelo RP2040, DMA, PIO e interrupções, o microcontrolador (133 MHz) é mais do que suficiente para o processamento do projeto, o DMA reduz a carga da CPU ao coletar dados do microfone, a matriz de LEDs usa PIO garantindo atualização rápida dos LEDs sem travamentos, ainda, as interrupções e a alocação dinâmica facilitam o processamento e ajudam a economizar recursos;
 - Durabilidade: Atendido pela seleção de componentes confiáveis.

## Software

1. Blocos funcionais – mostre um diagrama das camadas do software e suas funções.
![Máquina de Estados Principal](https://github.com/EduardoTejada/BitDorme/blob/main/img/maquina_de_estados.drawio.png?raw=true)  
Figura 2: Máquina de Estados Principal
  
![Máquina de Estados do Jogo da Memória](https://github.com/EduardoTejada/BitDorme/blob/main/img/maquina_de_estados_jogo_da_memoria.drawio.png?raw=true)  
Figura 3: Máquina de Estados do Jogo da Memória  
  
![Diagrama dos arquivos externos ao código principal e suas funções](https://github.com/EduardoTejada/BitDorme/blob/main/img/diagrama_software.drawio.png?raw=true)  
Figura 4: Diagrama dos arquivos externos ao código principal e suas funções
  
3. Descrição das funcionalidades – breve descrição das funções dos blocos de software.

Começando pela máquina de estados principal  
Estado 0: Exibe a tela inicial no display OLED mostrando a “logo” do projeto, troca de estado com uma interação no botão A (usa a função oled_draw);  
Estado 1: Utiliza uma função para fazer a seleção do horário, que utiliza o display OLED e o Joystick, passando para o próximo estado com o botão B;  
Estado 2: Utiliza a mesma função mencionada no estado 1 mas para a selecionar o horário que o alarme irá tocar, voltando pro estado anterior com o botão A e passando para o próximo com o botão B;  
Estado 3: Calcula o intervalo de tempo entre os horários atual e do alarme, aloca dinamicamente espaço na memória para salvar os dados do microfone a cada segundo e indica no display OLED o tempo de sono;  
Estado 4: Caso o usuário pressione o botão A volta ao estado 2, caso pressione o botão B salva os dados no RTC, configura o alarme para tocar, aciona o timer repetitivo a cada 1 segundo para coletar amostras do microfone e passa para o próximo estado;  
Estado 5: Estado de monitoramento de sono o código funciona somente por interrupções de callback do timer repetitivo, coletando amostras, salvando na alocação dinâmica e exibindo informações no display, finalmente, quando o callback do RTC é ativado significa que o alarme precisa tocar e então cancela-se o timer repetitivo e passa para o próximo estado;  
Estado 6: Ativa a matriz de LED’s e começa a tocar o som programado através do Buzzer até que algum botão seja pressionado, então passando para o próximo estado;  
Estado 7: Serve configurar o jogo, alocando a estrutura do tipo “memoryGame” que contém parâmetros do jogo da memória e sorteando a direção das flechas durante o jogo;  
Estado 8: Jogo da memória, o estado 8 é complexo e contém uma máquina de estados dentro dele mesmo (demonstrada nas imagens acima):  
  Subestado 0: Escreve na matriz de LED’s as flechas necessárias para o nível atual, por exemplo no nível 3 são exibidas 3 flechas (utilizando a função show_arrow e a lookup table para facilitar). Então ativa-se um alarme para os próximos 10 segundos e passa para o próximo subestado;  
  Subestado 1: Lê-se o joystick através da função get_joystick_position, caso não seja detectado movimento o alarme acionado no subestado anterior disparar, voltando a dois estados anteriores e tocando novamente o despertador. Caso tenha movimento no joystick, é indicado na matriz de LED’s, cancela-se o alarme e passa para o próximo subestado;  
  Subestado 2: Verifica-se se a flecha é igual a esperada para essa posição, em caso negativo exibe uma mensagem de “Você perdeu” no OLED e retorna pro estado anterior para configurar novamente o jogo. Em caso positivo, verifica se é a última flecha do nível (por exemplo, terceira flecha do terceiro nível), em caso negativo volta ao subestado 1, passa para detectar a próxima flecha e aciona o alarme dos 10 segundos novamente, em caso positivo verifica se passou por todos os níveis, em caso positivo passa para o próximo estado, em caso negativo zera a contagem de flechas, ativa o alarme de 10 segundos e volta ao subestado 1  
Estado 9: Lê o joystick através da função get_joystick_horizontal_pos, exibe os gráficos dos dados coletados anteriormente através da função oled_graph, como o display OLED é pequeno para exibir todos os dados utiliza-se o joystick para passar o gráfico para o lado, contudo, ainda é possível coletar os dados via UART e rodar o código em Python disponibilizado nos arquivos para gerar um gráfico pelo computador.  

3. Fluxograma – desenhe o fluxograma completo do software.  
![Fluxograma do algoritmo](https://github.com/EduardoTejada/BitDorme/blob/main/img/fluxograma_maquina_de_estados.drawio.png?raw=true) Figura 5: Fluxograma do algoritmo

## Execução do Projeto

1. Faça um vídeo de no máximo 3 minutos mostrando seu projeto funcionando. Inclua o link do vídeo no seu relatório de entrega.
[Apresentação e demonstração](https://youtu.be/SPbgMmyS5N0)
