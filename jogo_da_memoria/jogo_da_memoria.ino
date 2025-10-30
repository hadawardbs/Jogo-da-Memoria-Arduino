// C++ code - Jogo da memória estilo "Genius"
// -------------------------------------------
// O jogador precisa repetir a sequência de cores que o Arduino acende.
// A cada acerto, o nível aumenta e a sequência fica mais longa.

// -------------------------------------------
// ENUM: define nomes simbólicos para números inteiros
// -------------------------------------------
// Aqui damos nomes (VERMELHO, AZUL, etc.) para facilitar a leitura do código.
// Assim, em vez de usar 0, 1, 2, 3, usamos palavras com significado.
enum Cores { VERMELHO = 0, AZUL = 1, AMARELO = 2, VERDE = 3 };

// -------------------------------------------
// Definindo os pinos dos LEDs e botões
// -------------------------------------------
// Cada cor está associada a um pino do Arduino.
int leds[] = {2, 3, 4, 5};     // LEDs nas portas 2, 3, 4 e 5
int botoes[] = {6, 7, 8, 9};   // Botões nas portas 6, 7, 8 e 9

// Pino do buzzer (emite sons)
int buzzer = 10;

// -------------------------------------------
// Variáveis de controle do jogo
// -------------------------------------------
const int tamanhoSequencia = 20;   // Tamanho máximo da sequência (pode aumentar)
int sequencia[tamanhoSequencia];  // Vetor que guarda a ordem das cores
int nivel = 1;                    // Começa no nível 1 (1 cor por vez)

// -------------------------------------------
// SETUP: executa uma vez quando o Arduino liga
// -------------------------------------------
void setup()
{
  // Define o modo de cada pino (entrada ou saída)
  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);  // LEDs são saídas (Arduino controla)
    pinMode(botoes[i], INPUT); // Botões são entradas (Arduino lê)
  }

  pinMode(buzzer, OUTPUT);     // Buzzer também é uma saída

  // Gera uma sequência aleatória de cores para o jogo
  gerarSequenciaCores();

  // Inicia a comunicação serial (para exibir no monitor do PC)
  Serial.begin(9600);

  // Mostra no monitor serial qual sequência foi gerada (para testes)
  for (int i = 0; i < tamanhoSequencia; i++) {
    Serial.print(sequencia[i]);
    Serial.print(" ");
  }
  Serial.println();
}

// -------------------------------------------
// LOOP: roda repetidamente
// -------------------------------------------
void loop()
{
  // Se estiver no nível 1, espera o jogador apertar o botão vermelho para começar
  if (nivel == 1) {
    esperaBotaoStart();
  }

  delay(1000); // Pausa de 1 segundo antes da rodada começar

  mostrarSequenciaCores(); // Mostra a sequência até o nível atual
  lerBotoes();             // Lê a resposta do jogador

  // Se o jogador passar de todos os níveis, ele vence
  if (nivel > tamanhoSequencia) {
    vitoria();
  }
}

// -------------------------------------------
// Gera uma sequência aleatória de cores (números de 1 a 4)
// -------------------------------------------
void gerarSequenciaCores() {
  randomSeed(analogRead(A0)); // Usa ruído analógico para "aleatorizar" o random
  for (int i = 0; i < tamanhoSequencia; i++) {
    sequencia[i] = random(1, 5); // Gera um número entre 1 e 4 (cada cor)
  }
}

// -------------------------------------------
// Mostra a sequência de cores até o nível atual
// -------------------------------------------
void mostrarSequenciaCores() {
  for (int i = 0; i < nivel; i++) {
    int cor = sequencia[i] - 1; // Ajusta o índice (1–4 → 0–3)
    acenderLed(cor);            // Acende o LED correspondente
  }
}

// -------------------------------------------
// Acende o LED de uma determinada cor e toca um som
// -------------------------------------------
void acenderLed(int cor) {
  digitalWrite(leds[cor], HIGH);          // Liga o LED
  tone(buzzer, 800 + cor * 200, 100);    // Toca um som diferente por cor
  delay(300);                             // Espera um pouco
  digitalWrite(leds[cor], LOW);           // Desliga o LED
  delay(200);                             // Pausa antes do próximo
}

// -------------------------------------------
// Lê os botões pressionados e verifica se o jogador acertou
// -------------------------------------------
void lerBotoes() {
  int posicao = 0; // Posição atual na sequência

  // Continua até o jogador repetir todas as cores do nível
  while (posicao < nivel) {
    int corEsperada = sequencia[posicao] - 1; // Cor correta da vez
    int corPressionada = botaoPressionado();  // Verifica qual botão foi apertado

    if (corPressionada != -1) { // Se algum botão foi apertado
      tone(buzzer, 1000, 100);  // Faz um beep de confirmação

      //Verifica se apertou o botão certo
      if (corPressionada == corEsperada) {
        // Acertou a cor
        acenderLed(corPressionada);
        posicao++;
      } else {
        // Errou — perde o jogo
        derrota();
        resetarNivel();
        return; // Sai da função (reinicia o jogo)
      }

      // Espera até o jogador soltar o botão antes de continuar
      while (algumBotaoPressionado()) {}
    }
  }

  // Se terminou de repetir todas as cores corretamente, sobe o nível
  nivel++;
}

// -------------------------------------------
// Retorna o índice do botão pressionado (0 a 3) ou -1 se nenhum
// -------------------------------------------
int botaoPressionado() {
  for (int i = 0; i < 4; i++) {
    if (digitalRead(botoes[i]) == HIGH) return i;
  }
  return -1; // Nenhum botão apertado
}

// -------------------------------------------
// Efeitos de derrota
// -------------------------------------------
void derrota() {
  ligarTodosLeds();
  Serial.println("ERROU!");
  tone(buzzer, 300, 400);  // Som grave de erro
  delay(2000);
  desligarTodosLeds();
  delay(1000);
}

// -------------------------------------------
// Efeitos de vitória (quando termina o jogo)
// -------------------------------------------
void vitoria() {
  ligarTodosLeds();
  tocarMusicaVitoria(); // Toca musiquinha curta
  delay(2000);
  desligarTodosLeds();
  delay(1000);
  resetarNivel();        // Reinicia o jogo
}

// -------------------------------------------
// Pequena melodia de vitória (vários tons)
// -------------------------------------------
void tocarMusicaVitoria() {
  tone(buzzer, 1000, 120); delay(150);
  tone(buzzer, 1200, 120); delay(150);
  tone(buzzer, 1400, 120); delay(150);
  tone(buzzer, 1600, 250); delay(300);
  tone(buzzer, 1800, 400); delay(450);
}

// -------------------------------------------
// Liga todos os LEDs ao mesmo tempo
// -------------------------------------------
void ligarTodosLeds() {
  digitalWrite(leds[VERMELHO], HIGH);
  digitalWrite(leds[AZUL], HIGH);
  digitalWrite(leds[AMARELO], HIGH);
  digitalWrite(leds[VERDE], HIGH);
}

// -------------------------------------------
// Desliga todos os LEDs
// -------------------------------------------
void desligarTodosLeds() {
  digitalWrite(leds[VERMELHO], LOW);
  digitalWrite(leds[AZUL], LOW);
  digitalWrite(leds[AMARELO], LOW);
  digitalWrite(leds[VERDE], LOW);
}

// -------------------------------------------
// Reinicia o jogo (nível 1 e nova sequência)
// -------------------------------------------
void resetarNivel() {
  nivel = 1;
  gerarSequenciaCores();
}

// -------------------------------------------
// Espera o jogador apertar o botão vermelho para começar
// -------------------------------------------
void esperaBotaoStart() {
  digitalWrite(leds[VERMELHO], HIGH);         // LED vermelho indica "pronto"
  while (digitalRead(botoes[VERMELHO]) == LOW) {}  // Espera apertar
  tone(buzzer, 1000, 100);                    // Som de confirmação
  while (digitalRead(botoes[VERMELHO]) == HIGH) {} // Espera soltar
  digitalWrite(leds[VERMELHO], LOW);          // Apaga LED
}

// -------------------------------------------
// Verifica se qualquer botão está pressionado
// -------------------------------------------
bool algumBotaoPressionado() {
  for (int i = 0; i < 4; i++) {
    if (digitalRead(botoes[i]) == HIGH) return true;
  }
  return false;
}
