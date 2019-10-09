/////////////versão com erro
agora foi! 5
/////////////

O arquivo "agora foi! 5" é um programa da -Tomada monitorada wifi V5- ultilizando um NODEMCU ESP32.

Este projeto tem a finalidade do usuario de monitorar seu consumo de energia e poder acionar seu equipamento através da rede local wifi.
Como ainda está em FASE DE TESTES, está sendo ultilizado um sensor LDR.

Este código foi criado atraves da base de diversos códigos:

Youtube -  Canal G6EJD (https://www.youtube.com/channel/UCgtlqH_lkMdIa4jZLItcsTg)
FilipeFlop - Planta Iot (https://www.filipeflop.com/blog/planta-iot-com-esp8266-nodemcu/)
	   - Automação Residencial com Arduino (https://www.filipeflop.com/blog/automacao-residencial-com-arduino-acenda-lampadas-pela-internet/)
Fernando K - ESP8266: como monitorar temperatura e umidade (https://www.fernandok.com/2017/10/blog-post.html)
	   - Não perca tempo: use NTP! (https://www.fernandok.com/2018/12/nao-perca-tempo-use-ntp.html)

ERROS
**1 - Nesta versão 5, o gráfico do ThinkSpeak não funciona, (hipotese: não está recebendo ou enviando os dados). 
**2 - Protocolo NTP tambem não funcionou, pois este seria ultilizado para criar um data logger de contagem de minutos/horas no dia em que o sensor marcou acima de 0A
para a memória interna do Nodemcu. Assim poderia se calcular o valor previsto e atual de consumo.(não esta agregado na versão agorafoi!5 e nem na agorafoi!4).
**3 - Nesta versão tambem há um problema com o JavaScript/CSS. Deveria mostrar uma lampada de identificação
quando o equipamento/lampada estiver ligado (acesa) e desligada(apagada). O codigo base foi ultilizado -Automação Residencial com Arduino- do FilipeFlop.


////////////////////versão ok
thinkSpeak3(small)
///////////////////

Código baseado no -Planta Iot- do FilipeFlop, foi otimizado e agregado ao codigo principal "agora foi! 5"


//////////////////////////versões ok
NTP1ok, NTP2ok e NTP3ok
/////////////////////////

Códigos baseados em diferentes, sendo NTP3 do Fernando K. Todos os três foram otimizados para serem agregados na versão 4 do projeto "agora foi!4".
Separadamente todos funcionam, porem quando foram agregadas, nenhum dos NTPxok funcionoou com a versão 4, onde elas entraram em conflito.


/////////////versão ok
agora foi! 4
/////////////

Versão anterior funciona corretamente acionando o rele, medindo o sensor e fazendo os calculos.
Ela NÃO conta com a identificação de lampada acesa ou apagada(apenas uma lampada apagada para demarcar lugar), não há grafico, e não há relogio. 
 