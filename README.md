# Edge-Computing-IoT

# AquaDefend - Sistema de Monitoramento de Nível de Água com Alerta Simulado - GlobalSolution

[![Wokwi](https://img.shields.io/badge/Simular%20no-Wokwi-4BC0A4?style=flat&logo=wokwi)](https://wokwi.com/projects/432297116988662785)

## 1. Descrição do Problema

O monitoramento contínuo do nível de água em reservatórios, rios ou áreas suscetíveis a inundações é crucial para a prevenção de desastres e gestão de recursos hídricos. A falta de um sistema automatizado e acessível pode levar a respostas tardias em situações críticas, como enchentes, colocando vidas e propriedades em risco. Além disso, a ausência de registros históricos dificulta a análise de padrões e a tomada de decisões preventivas.

Este projeto visa solucionar esse problema através da criação de um protótipo de baixo custo, utilizando a plataforma Arduino, para:

*   Medir o nível da água continuamente.
*   Exibir o status atual (Seguro, Alerta, Perigo) e informações relevantes (distância, data/hora) em um display LCD.
*   Fornecer alertas visuais imediatos através de LEDs.
*   Registrar a data e hora exatas dos eventos.
*   Simular o envio de alertas detalhados para órgãos competentes (ex: Defesa Civil) e para uma plataforma online de visualização (mapa de alertas) quando um nível de perigo é detectado.

## 2. Visão Geral da Solução

O AquaDefend utiliza um **Arduino UNO** como unidade central de processamento. Um sensor ultrassônico **HC-SR04** mede a distância até a superfície da água. Um display **LCD I2C 20x4** exibe a distância, o status do nível e a data/hora atual, fornecida por um módulo **RTC DS1307** para garantir a precisão temporal.

Três **LEDs** (verde, amarelo e vermelho) indicam visualmente o status:

*   **Verde:** Nível Seguro (Distância < 150 cm)
*   **Amarelo:** Nível de Alerta (150 cm <= Distância < 200 cm)
*   **Vermelho:** Nível de Perigo (Distância >= 200 cm)

Ao entrar em estado de **PERIGO**, o sistema executa as seguintes ações:

1.  Acende o LED vermelho.
2.  Exibe "PERIGO!" no LCD.
3.  Mostra a mensagem temporária "ALERTA SIMULADO!" no LCD.
4.  Imprime no **Serial Monitor** uma simulação detalhada de uma requisição HTTP POST que seria enviada para uma API (`api.aquadefend`), contendo o timestamp e a mensagem de perigo. A simulação também inclui mensagens indicando o envio para a Defesa Civil e para um mapa online.

O sistema conta com uma sequência de inicialização que exibe um logo customizado (fornecido pelo usuário) de forma estática, seguido pela revelação animada (letra por letra) do nome "AquaDefend" e do slogan "Monitoramento Nivel".

### Componentes Utilizados

*   1x Arduino UNO R3 (ou compatível)
*   1x Sensor Ultrassônico HC-SR04
*   1x Display LCD I2C 20x4 (com módulo PCF8574)
*   1x Módulo Relógio de Tempo Real (RTC) DS1307
*   1x LED Verde (5mm)
*   1x LED Amarelo (5mm)
*   1x LED Vermelho (5mm)
*   3x Resistor 220 Ohms
*   1x Protoboard (Placa de Ensaio)
*   Jumpers (fios de conexão macho-macho)

### Diagrama da Montagem



![Gs-Arduino](https://github.com/user-attachments/assets/7a9060b2-fb3c-4adf-aa30-a3c9acf34acb)




**Tabela de Conexões:**

| Componente         | Pino Componente | Conexão Arduino | Observação                     |
| :----------------- | :-------------- | :-------------- | :----------------------------- |
| **LCD I2C**        | GND             | GND             |                                |
|                    | VCC             | 5V              |                                |
|                    | SDA             | A4              | Pinos I2C                      |
|                    | SCL             | A5              | Pinos I2C                      |
| **RTC DS1307**     | GND             | GND             |                                |
|                    | VCC             | 5V              |                                |
|                    | SDA             | A4              | Pode compartilhar com o LCD    |
|                    | SCL             | A5              | Pode compartilhar com o LCD    |
| **HC-SR04**        | GND             | GND             |                                |
|                    | VCC             | 5V              |                                |
|                    | Trig            | D9              | Pino Digital 9                 |
|                    | Echo            | D8              | Pino Digital 8                 |
| **LED Verde**      | Catodo (-)      | GND             |                                |
|                    | Anodo (+)       | Resistor 220Ω   | Resistor em série              |
| *Resistor (Verde)* | Outro lado      | D6              | Pino Digital 6                 |
| **LED Amarelo**    | Catodo (-)      | GND             |                                |
|                    | Anodo (+)       | Resistor 220Ω   | Resistor em série              |
| *Resistor (Amarelo)*| Outro lado      | D7              | Pino Digital 7                 |
| **LED Vermelho**   | Catodo (-)      | GND             |                                |
|                    | Anodo (+)       | Resistor 220Ω   | Resistor em série              |
| *Resistor (Vermelho)*| Outro lado     | D10             | Pino Digital 10                |

## 3. Guia para Simular o Projeto no Wokwi

Wokwi é um simulador online gratuito e poderoso para Arduino, ESP32 e outros microcontroladores. Siga estes passos para simular o AquaDefend:

1.  **Acesse o Projeto:** Clique no link abaixo para abrir a simulação diretamente no Wokwi:
    *   ➡️ **[Simular AquaDefend no Wokwi](https://wokwi.com/projects/432297116988662785)** ⬅️

2.  **Explore o Ambiente:**
    *   À esquerda, você verá o diagrama do circuito montado.
    *   À direita, você encontrará o código (`sketch.ino`) e a janela do Serial Monitor.

3.  **Ajuste Inicial da Hora (Opcional):** Se for a primeira vez que você roda a simulação ou se a hora parecer incorreta:
    *   No código (`sketch.ino`), localize a linha: `// rtc.adjust(DateTime(2025, 5, 30, 19, 40, 0));`
    *   Remova o `//` do início.
    *   Altere os números para representar o ano, mês, dia, hora, minuto e segundo atuais.
    *   Clique no botão verde ▶️ ("Start simulation"). Deixe rodar por 5-10 segundos.
    *   Clique no botão vermelho ⏹️ ("Stop simulation").
    *   Adicione o `//` de volta no início da linha `rtc.adjust(...)`.
    *   Isso garante que a hora seja ajustada apenas uma vez.

4.  **Inicie a Simulação:** Clique no botão verde ▶️ ("Start the simulation").

5.  **Observe:**
    *   **LCD:** Veja a sequência de inicialização (logo estático, textos animados) e, em seguida, a exibição da distância, nível e timestamp.
    *   **Serial Monitor:** Acompanhe as mensagens de inicialização e as leituras periódicas de distância e status.
    *   **LEDs:** Veja qual LED está aceso de acordo com o nível.

6.  **Interaja com o Sensor:**
    *   Clique no componente **HC-SR04** no diagrama.
    *   Um controle deslizante aparecerá.
    *   Arraste o controle para alterar a distância simulada:
        *   **< 150 cm:** LED Verde aceso, LCD mostra "Seguro".
        *   **150-199 cm:** LED Amarelo aceso, LCD mostra "Alerta!".
        *   **>= 200 cm:** LED Vermelho aceso, LCD mostra "PERIGO!", a mensagem "ALERTA SIMULADO!" pisca no LCD, e a simulação do envio HTTP aparece no Serial Monitor.

## 4. Vídeo Demonstrativo

Assista a uma demonstração do AquaDefend em ação:

```
[INSERIR LINK DO VÍDEO DEMONSTRATIVO AQUI]

```

## 5. Código Fonte

O código fonte completo e comentado para este projeto está disponível neste repositório:

*   [`aqua_defend_v1.0.ino`](./aqua_defend_v1.ino)

O código segue boas práticas de programação, incluindo:

*   **Comentários:** Explicações detalhadas sobre as seções, lógica e funções.
*   **Nomenclatura Clara:** Variáveis e funções com nomes descritivos.
*   **Indentação Adequada:** Código formatado para facilitar a leitura.
*   **Estrutura Lógica:** Funções bem definidas para modularizar o código (`setup`, `loop`, `runStartupSequence`, `getTimestamp`, `sendSerialAlertSimulation`).
*   **Uso de `const` e `#define`:** Para constantes e pinos, melhorando a manutenção.
*   **Uso de `F()` Macro:** Para armazenar strings constantes na memória Flash, economizando RAM.

---
## 6. Integrantes

-Diogo Pelinson Duarte de Moraes

-Lorenzo Andolfatto Coque

-Pedro Henrique Caires da Silva
