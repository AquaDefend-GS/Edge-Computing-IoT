/********* 
 * AquaDefend - Sistema de Monitoramento de Nível de Água com Alerta Simulado
 * 
 * Versão: 4.6 (Wokwi - Logo Estático + Anim Texto + Sim Serial Limpo + Ajuste Status)
 * Autor: Diogo, Pedro e Lorenzo
 * Data: 30/05/2025
 * 
 * Descrição: 
 * Este projeto utiliza um Arduino UNO para monitorar o nível da água usando um sensor 
 * ultrassônico HC-SR04. O nível é exibido em um display LCD I2C 20x4, juntamente 
 * com a data e hora atuais obtidas de um módulo RTC DS1307. LEDs indicam o status 
 * do nível (Seguro, Alerta, Perigo). Ao atingir o nível de Perigo, o sistema 
 * simula o envio de um alerta para uma central (ex: Defesa Civil) e para um mapa 
 * online, exibindo os detalhes da comunicação no Serial Monitor.
 * A inicialização inclui a exibição de um logo customizado e uma animação de texto.
 *********/

// --- Bibliotecas --- 
#include <Wire.h>              // Necessária para comunicação I2C (LCD e RTC)
#include <LiquidCrystal_I2C.h> // Biblioteca para o Display LCD com módulo I2C
#include <RTClib.h>            // Biblioteca para o Módulo RTC (DS1307/DS3231)

// --- Definições de Pinos --- 
// Sensor Ultrassônico
#define trigPin 9       // Pino de Trigger do HC-SR04
#define echoPin 8       // Pino de Echo do HC-SR04

// LEDs de Status
#define ledVerde 2     // Pino para o LED Verde (Nível Seguro)
#define ledAmarelo 3   // Pino para o LED Amarelo (Nível de Alerta)
#define ledVermelho 4    // Pino para o LED Vermelho (Nível de Perigo)

// --- Configuração dos Módulos --- 
// Inicializa o LCD I2C: Endereço 0x27, 20 colunas, 4 linhas
LiquidCrystal_I2C lcd(0x27, 20, 4); 
// Inicializa o objeto RTC para o modelo DS1307
RTC_DS1307 rtc;

// --- Variáveis Globais --- 
long duration;          // Armazena a duração do pulso do sensor HC-SR04 (microsegundos)
float distance;         // Armazena a distância calculada (em centímetros)
char timestampBuffer[20]; // Buffer para armazenar a string formatada de data/hora
boolean perigoState = false; // Flag para controlar se o alerta de perigo já foi enviado (evita envios repetidos)

// --- Definições dos Caracteres Customizados (LOGO DO USUÁRIO 3x2) ---
// Array de bytes definindo os pixels de cada caractere customizado (5x8 pixels)
// Estes foram fornecidos pelo usuário.
byte name0x7[] = { B00011, B00100, B01000, B10000, B10000, B10000, B10001, B10001 }; // Parte Superior Esquerda
byte name0x8[] = { B11111, B00000, B00100, B01010, B10001, B10001, B00000, B00000 }; // Parte Superior Meio
byte name0x9[] = { B11000, B00100, B00010, B00001, B00001, B00001, B10001, B10001 }; // Parte Superior Direita
byte name1x7[] = { B10001, B10001, B10001, B10000, B10000, B01000, B00100, B00011 }; // Parte Inferior Esquerda
byte name1x8[] = { B00000, B00000, B00000, B10001, B01110, B00000, B00000, B11111 }; // Parte Inferior Meio
byte name1x9[] = { B10001, B10001, B10001, B00001, B00001, B00010, B00100, B11000 }; // Parte Inferior Direita

// --- Constantes para Simulação de API --- 
const char* API_HOST = "api.aquadefend.sim"; // Host fictício para simulação de envio
const char* API_ENDPOINT = "/alerta";      // Endpoint fictício para simulação de envio

// --- Protótipos de Funções --- 
// Declaração das funções que serão definidas posteriormente
void getTimestamp();
void sendSerialAlertSimulation(const char* alertMessage);
void runStartupSequence(); 

// --- Função setup() --- 
// Executada uma vez quando o Arduino é ligado ou resetado
void setup() {
  // Inicializa a comunicação serial (para debug e simulação de alerta)
  Serial.begin(115200); 
  while (!Serial) { ; } // Espera a porta serial conectar (necessário para alguns Arduinos)
  Serial.println(F("\nIniciando AquaDefend V1.0."));

  // Configura os pinos dos LEDs e do sensor como OUTPUT ou INPUT
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  Serial.println(F("- Pinos configurados."));

  // Inicializa o LCD, liga o backlight e limpa a tela
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.println(F("- LCD I2C 20x4 inicializado."));

  // Cria os caracteres customizados na memória do LCD
  // Associa cada array de bytes a um índice (1 a 6)
  lcd.createChar(1, name0x7);  
  lcd.createChar(2, name0x8);  
  lcd.createChar(3, name0x9);  
  lcd.createChar(4, name1x7);  
  lcd.createChar(5, name1x8);  
  lcd.createChar(6, name1x9);  
  Serial.println(F("- Caracteres customizados, criados."));

  // Inicializa o módulo RTC
  if (!rtc.begin()) {
    Serial.println(F("ERRO: RTC nao encontrado! Verifique as conexoes I2C (A4/A5)."));
    lcd.setCursor(0,0);
    lcd.print(F("Erro RTC!"));
    while (1) delay(10); // Trava a execução se o RTC não for encontrado
  }
  Serial.println(F("- RTC DS1307 inicializado."));

  // Ajuste inicial da hora do RTC (se necessário)
  // Remova o comentário da linha abaixo e ajuste a data/hora na primeira vez que rodar
  // Exemplo: rtc.adjust(DateTime(ANO, MES, DIA, HORA, MINUTO, SEGUNDO));
  // rtc.adjust(DateTime(2025, 5, 30, 19, 40, 0)); 

  // --- Executa a Sequência de Inicialização (Logo Estático + Texto Animado) --- 
  runStartupSequence();

  Serial.println(F("Setup concluido. Iniciando loop principal."));
}

// --- Função da Sequência de Inicialização --- 
// Exibe o logo e os textos iniciais no LCD
void runStartupSequence() {
  Serial.println(F("- Iniciando sequencia de inicializacao..."));
  lcd.clear();
  
  // Define as posições para centralizar o logo e os textos
  int logoCol = 8; // Coluna inicial do logo (8 para centralizar 3 caracteres em 20 colunas)
  int logoRow = 1; // Linha inicial do logo (1 para centralizar 2 linhas em 4)
  int textCol = 5; // Coluna inicial do título "AquaDefend" (5 para centralizar 10 caracteres)
  int sloganCol = 0; // Coluna inicial do slogan "Monitoramento Nivel"
  const char* title = "AquaDefend"; 
  const char* slogan = "Monitoramento Nivel"; 

  // 1. Desenha o logo estático no centro do LCD
  lcd.setCursor(logoCol, logoRow);     lcd.write(1); // Exibe o caractere customizado 1
  lcd.setCursor(logoCol + 1, logoRow); lcd.write(2); // Exibe o caractere customizado 2
  lcd.setCursor(logoCol + 2, logoRow); lcd.write(3); // Exibe o caractere customizado 3
  lcd.setCursor(logoCol, logoRow + 1); lcd.write(4); // Exibe o caractere customizado 4
  lcd.setCursor(logoCol + 1, logoRow + 1); lcd.write(5); // Exibe o caractere customizado 5
  lcd.setCursor(logoCol + 2, logoRow + 1); lcd.write(6); // Exibe o caractere customizado 6
  delay(1000); // Pausa curta (1 segundo) após exibir o logo

  // 2. Revela o título (letra por letra)
  lcd.setCursor(textCol, 0); // Posiciona o cursor para o título na linha 0
  for (int i = 0; i < strlen(title); i++) { // Itera sobre cada caractere do título
    lcd.print(title[i]); // Imprime o caractere atual
    delay(150); // Pequena pausa entre as letras para efeito de digitação
  }

  // 3. Revela o slogan (letra por letra)
  lcd.setCursor(sloganCol, 3); // Posiciona o cursor para o slogan na linha 3
  for (int i = 0; i < strlen(slogan); i++) { // Itera sobre cada caractere do slogan
    lcd.print(slogan[i]); // Imprime o caractere atual
    delay(150); // Pequena pausa entre as letras
  }

  delay(3000); // Pausa longa (3 segundos) para visualização da tela completa
  lcd.clear(); // Limpa o LCD antes de iniciar o monitoramento
  Serial.println(F("- Sequencia de inicializacao concluida."));
}


// --- Função getTimestamp() --- 
// Obtém a data e hora atuais do RTC e formata em uma string
void getTimestamp() {
  DateTime now = rtc.now(); // Lê a data/hora atual do módulo RTC
  // Formata a data/hora no buffer: DD/MM/AA HH:MM:SS
  sprintf(timestampBuffer, "%02d/%02d/%02d %02d:%02d:%02d", 
          now.day(), now.month(), now.year() % 100, // Dia, Mês, Ano (últimos 2 dígitos)
          now.hour(), now.minute(), now.second());   // Hora, Minuto, Segundo
}

// --- Função sendSerialAlertSimulation() --- 
// Imprime no Serial Monitor uma simulação de requisição HTTP POST
void sendSerialAlertSimulation(const char* alertMessage) {
  Serial.println(F("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
  Serial.println(F(">>> SIMULANDO ENVIO HTTP PARA api.aquadefend <<<"));
  getTimestamp(); // Atualiza o timestamp para o momento do alerta
  
  // Monta o corpo da requisição JSON (exemplo)
  String postData = "{\"timestamp\":\"" + String(timestampBuffer) + "\", \"message\":\"" + String(alertMessage) + "\"}";
  
  // Imprime a simulação da requisição HTTP
  Serial.println(F("POST /alerta HTTP/1.1"));
  Serial.print(F("Host: ")); Serial.println(API_HOST);
  Serial.println(F("Content-Type: application/json"));
  Serial.print(F("Content-Length: ")); Serial.println(postData.length());
  Serial.println(); // Linha em branco obrigatória entre cabeçalho e corpo
  Serial.println(postData); // Corpo da requisição
  Serial.println(F("------------------------------------------------------------"));
  // Mensagens adicionais simulando comunicação com outros órgãos/sistemas
  Serial.println(F("[SIM] Comunicado enviado para Defesa Civil SP (199)."));
  Serial.println(F("[SIM] Comunicado enviado para atualizacao do mapa em www.aquadefend/mapa."));
  Serial.println(F(">>> FIM DA SIMULACAO DE ENVIO <<<"));
  Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
}

// --- Função loop() --- 
// Executada repetidamente após o setup()
void loop() {
  // 1. Medição da Distância com HC-SR04
  digitalWrite(trigPin, LOW); // Garante que o pino Trig esteja em nível baixo
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // Envia um pulso de 10 microsegundos no pino Trig
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Lê a duração do pulso de retorno no pino Echo (em microsegundos)
  // Timeout de 25000 microsegundos (25ms) para evitar travamentos se não houver retorno
  duration = pulseIn(echoPin, HIGH, 25000); 
  
  // Calcula a distância em centímetros
  // Fórmula: (duração * velocidade_som / 2)
  // Velocidade do som ~ 0.034 cm/microsegundo
  // Divide por 2 porque o tempo medido é de ida e volta do som
  // Se duration for 0 (timeout), define distance como -1 para indicar erro
  distance = (duration > 0) ? (duration * 0.034) / 2 : -1; 
  
  // 2. Debug no Serial Monitor
  getTimestamp(); // Atualiza a data/hora
  Serial.print(timestampBuffer); // Imprime data/hora
  Serial.print(F(" - Dist: "));
  if (distance == -1) {
    Serial.print(F("Erro/Timeout")); // Informa erro na leitura do sensor
  } else {
    Serial.print(distance, 1); // Imprime a distância com 1 casa decimal
    Serial.print(F(" cm"));
  }

  // 3. Atualização do Display LCD 20x4
  // Linha 0: Distância
  lcd.setCursor(0, 0); 
  lcd.print(F("Distancia:          ")); // Limpa a área da distância anterior
  lcd.setCursor(11, 0); 
  if (distance == -1) {
    lcd.print(F("Erro Sens")); // Mostra erro no LCD
  } else {
    lcd.print(distance, 1); // Mostra a distância
    lcd.print(F(" cm   ")); // Adiciona "cm" e espaços para limpar o resto
  }

  // Linha 1: Nível
  lcd.setCursor(0, 1); 
  lcd.print(F("Nivel:              ")); // Limpa a área do nível anterior
  lcd.setCursor(7, 1); // Posição para exibir o status do nível

  // Linha 3: Timestamp
  lcd.setCursor(0, 3); 
  lcd.print(timestampBuffer); // Mostra data/hora atual
  lcd.print(F("   ")); // Limpa o restante da linha

  // 4. Lógica de Status (LEDs, Mensagem LCD, Alerta)
  // Apaga todos os LEDs antes de verificar o status atual
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVermelho, LOW);
  
  const char* statusMessage = ""; // Mensagem para o Serial Monitor
  boolean currentPerigo = false; // Flag para indicar se o nível atual é de perigo
  const char* lcdStatus = "";   // Mensagem para o LCD

  // Define o status baseado na distância medida
  if (distance == -1) { 
    // Caso de erro na leitura do sensor
    lcdStatus = "Falha Sensor"; 
    statusMessage = "Falha no Sensor";
    perigoState = false; // Reseta o estado de perigo se o sensor falhar
  } else if (distance < 150) { 
    // Nível Seguro (0-149 cm)
    digitalWrite(ledVerde, HIGH);
    lcdStatus = "Seguro   "; 
    statusMessage = "Nivel Seguro";
    perigoState = false; // Reseta o estado de perigo
  } else if (distance >= 150 && distance < 200) { 
    // Nível de Alerta (150-199 cm) - AJUSTADO
    digitalWrite(ledAmarelo, HIGH);
    lcdStatus = "Alerta!  "; // AJUSTADO de "Atencao! "
    statusMessage = "Nivel de Alerta"; // AJUSTADO de "Nivel de Atencao"
    perigoState = false; // Reseta o estado de perigo
  } else { 
    // Nível de Perigo (200+ cm)
    digitalWrite(ledVermelho, HIGH);
    lcdStatus = "PERIGO!  "; 
    statusMessage = "Nivel de PERIGO";
    currentPerigo = true; // Define a flag de perigo atual
  }

  // Exibe o status do nível no LCD
  lcd.setCursor(7, 1); 
  lcd.print(lcdStatus);

  // Lógica para simular o envio de alerta
  // O alerta só é enviado na TRANSIÇÃO para o estado de PERIGO
  if (currentPerigo && !perigoState) {
    perigoState = true; // Marca que o alerta foi (simulado) enviado para este evento
    
    // Monta a mensagem específica do alerta
    char alertLogBuffer[30]; 
    dtostrf(distance, 4, 1, alertLogBuffer + 15); // Converte a distância float para string
    sprintf(alertLogBuffer, "PERIGO! Dist: %s cm", alertLogBuffer + 15);
    
    // Chama a função que simula o envio do alerta no Serial Monitor
    sendSerialAlertSimulation(alertLogBuffer); 

    // Mostra uma mensagem temporária no LCD indicando o alerta
    lcd.setCursor(0, 2); 
    lcd.print(F("ALERTA SIMULADO!    "));
    delay(2500); // Mantém a mensagem por 2.5 segundos
    lcd.setCursor(0, 2); 
    lcd.print(F("                    ")); // Limpa a linha 2 do LCD
  }
  
  // Imprime o status final no Serial Monitor
  Serial.print(F(" - Status: ")); 
  Serial.println(statusMessage);
  
  // Pausa principal do loop (7 segundos entre cada leitura)
  delay(7000); 
}

