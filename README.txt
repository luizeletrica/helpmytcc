/////////////vers�o com erro
agora foi! 5
/////////////

O arquivo "agora foi! 5" � um programa da -Tomada monitorada wifi V5- ultilizando um NODEMCU ESP32.

Este projeto tem a finalidade do usuario de monitorar seu consumo de energia e poder acionar seu equipamento atrav�s da rede local wifi.
Como ainda est� em FASE DE TESTES, est� sendo ultilizado um sensor LDR.

Este c�digo foi criado atraves da base de diversos c�digos:

Youtube -  Canal G6EJD (https://www.youtube.com/channel/UCgtlqH_lkMdIa4jZLItcsTg)
FilipeFlop - Planta Iot (https://www.filipeflop.com/blog/planta-iot-com-esp8266-nodemcu/)
	   - Automa��o Residencial com Arduino (https://www.filipeflop.com/blog/automacao-residencial-com-arduino-acenda-lampadas-pela-internet/)
Fernando K - ESP8266: como monitorar temperatura e umidade (https://www.fernandok.com/2017/10/blog-post.html)
	   - N�o perca tempo: use NTP! (https://www.fernandok.com/2018/12/nao-perca-tempo-use-ntp.html)

ERROS
**1 - Nesta vers�o 5, o gr�fico do ThinkSpeak n�o funciona, (hipotese: n�o est� recebendo ou enviando os dados). 
**2 - Protocolo NTP tambem n�o funcionou, pois este seria ultilizado para criar um data logger de contagem de minutos/horas no dia em que o sensor marcou acima de 0A
para a mem�ria interna do Nodemcu. Assim poderia se calcular o valor previsto e atual de consumo.(n�o esta agregado na vers�o agorafoi!5 e nem na agorafoi!4).
**3 - Nesta vers�o tambem h� um problema com o JavaScript/CSS. Deveria mostrar uma lampada de identifica��o
quando o equipamento/lampada estiver ligado (acesa) e desligada(apagada). O codigo base foi ultilizado -Automa��o Residencial com Arduino- do FilipeFlop.


////////////////////vers�o ok
thinkSpeak3(small)
///////////////////

C�digo baseado no -Planta Iot- do FilipeFlop, foi otimizado e agregado ao codigo principal "agora foi! 5"


//////////////////////////vers�es ok
NTP1ok, NTP2ok e NTP3ok
/////////////////////////

C�digos baseados em diferentes, sendo NTP3 do Fernando K. Todos os tr�s foram otimizados para serem agregados na vers�o 4 do projeto "agora foi!4".
Separadamente todos funcionam, porem quando foram agregadas, nenhum dos NTPxok funcionoou com a vers�o 4, onde elas entraram em conflito.


/////////////vers�o ok
agora foi! 4
/////////////

Vers�o anterior funciona corretamente acionando o rele, medindo o sensor e fazendo os calculos.
Ela N�O conta com a identifica��o de lampada acesa ou apagada(apenas uma lampada apagada para demarcar lugar), n�o h� grafico, e n�o h� relogio. 
 