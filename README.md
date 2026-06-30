# 🌱 Estufa Inteligente com ESP32

Sistema embarcado para monitoramento e controle de uma estufa
inteligente utilizando **ESP32**, **MQTT**, **Ubidots** e **Wokwi**.

Este projeto foi desenvolvido como trabalho da disciplina de
**Microcontroladores e Microprocessadores**, com o objetivo de
demonstrar a aplicação de sensores, atuadores e Internet das Coisas
(IoT) em um sistema de automação agrícola.

------------------------------------------------------------------------

# 📖 Sobre o Projeto

A estufa inteligente realiza o monitoramento contínuo das condições
ambientais e toma decisões automaticamente para manter um ambiente
adequado ao cultivo.

O ESP32 é responsável por:

-   Ler os sensores instalados na estufa;
-   Processar os dados recebidos;
-   Executar regras de automação;
-   Acionar os atuadores quando necessário;
-   Exibir informações em um display LCD;
-   Enviar todos os dados para o Ubidots utilizando o protocolo MQTT.

Todo o sistema foi desenvolvido e testado utilizando o simulador
**Wokwi**, integrado ao **PlatformIO** no Visual Studio Code.

------------------------------------------------------------------------

# 🎯 Objetivos

-   Monitorar temperatura, umidade do ar, umidade do solo e
    luminosidade;
-   Automatizar a irrigação da estufa;
-   Controlar a ventilação quando houver temperatura elevada;
-   Acionar iluminação artificial em ambientes com pouca luz;
-   Emitir alertas em situações críticas;
-   Disponibilizar todas as informações em tempo real através do
    Ubidots.

------------------------------------------------------------------------

# ⚙ Tecnologias Utilizadas

-   ESP32
-   C++
-   Arduino Framework
-   PlatformIO
-   Wokwi Simulator
-   MQTT
-   Ubidots
-   LCD I2C
-   Visual Studio Code

------------------------------------------------------------------------

# 🔧 Componentes Utilizados

## Sensores

  Componente                                  Função
  ------------------------------------------- -----------------------------
  DHT22                                       Temperatura e umidade do ar
  Sensor de Umidade do Solo (Potenciômetro)   Mede a umidade do solo
  LDR                                         Mede a luminosidade

## Atuadores

  Componente     Função
  -------------- --------------------------------
  Relé           Aciona a bomba de irrigação
  LED Verde      Simula a bomba de irrigação
  LED Vermelho   Simula o sistema de ventilação
  LED Amarelo    Simula a iluminação artificial
  Buzzer         Alarme sonoro
  LCD 16x2 I2C   Interface local do sistema

------------------------------------------------------------------------

# 🏗 Arquitetura

``` text
Sensores
(DHT22 | Solo | LDR)
          │
          ▼
        ESP32
          │
 ┌────────┼────────┐
 ▼        ▼        ▼
Bomba  Cooler  Iluminação
          │
        Buzzer
          │
          ▼
     LCD + MQTT
          │
          ▼
       Ubidots
```

------------------------------------------------------------------------

# 🧠 Lógica do Sistema

O ESP32 executa continuamente o seguinte ciclo:

``` text
Ler sensores
      ↓
Processar dados
      ↓
Tomar decisões
      ↓
Acionar atuadores
      ↓
Atualizar LCD
      ↓
Enviar dados ao Ubidots
      ↓
Repetir continuamente
```

## Regras implementadas

### Irrigação

-   Solo \< 35% → Liga bomba.
-   Solo entre 35% e 55% → Irriga apenas se:
    -   Temperatura \< 30°C
    -   Luminosidade \< 60%
    -   Umidade do ar \< 75%
-   Solo \> 55% → Bomba desligada.

### Ventilação

-   Temperatura \> 30°C → Liga cooler.

### Iluminação

-   Luminosidade \< 35% → Liga iluminação artificial.

### Alerta

-   Temperatura \> 35°C ou solo \< 25% → Aciona buzzer.

------------------------------------------------------------------------

# 📟 LCD

O display alterna automaticamente entre quatro telas:

1.  Umidade do solo e bomba.
2.  Temperatura e umidade do ar.
3.  Luminosidade e iluminação artificial.
4.  Estado da estufa, cooler e MQTT.

------------------------------------------------------------------------

# ☁ Integração com Ubidots

Variáveis enviadas via MQTT a cada 3 segundos:

-   temperatura
-   umidade_ar
-   umidade_solo
-   luminosidade
-   bomba
-   cooler
-   luz_artificial
-   alerta

------------------------------------------------------------------------

# 📂 Estrutura do Projeto

``` text
📦 Estufa-Inteligente
├── src/
│   └── main.cpp
├── include/
├── lib/
├── test/
├── diagram.json
├── platformio.ini
├── wokwi.toml
├── README.md
└── .gitignore
```

------------------------------------------------------------------------

# 🚀 Como Executar

1.  Clone o repositório.
2.  Abra no VS Code com PlatformIO.
3.  Configure seu TOKEN do Ubidots.
4.  Execute a simulação no Wokwi.
5.  Acompanhe os dados no LCD e no dashboard do Ubidots.

------------------------------------------------------------------------

# 🧪 Testes

-   ✅ Irrigação automática
-   ✅ Ventilação automática
-   ✅ Iluminação automática
-   ✅ Alarme
-   ✅ LCD
-   ✅ Comunicação MQTT
-   ✅ Dashboard Ubidots

------------------------------------------------------------------------

# 📸 Imagens

Adicione posteriormente:

-   Circuito no Wokwi
-   Dashboard do Ubidots
-   GIF da simulação

------------------------------------------------------------------------

# 🔮 Melhorias Futuras

-   Controle manual via Ubidots
-   Histórico de eventos
-   Sensor de nível de água
-   Sensor de CO₂
-   Controle por histerese
-   Aplicativo móvel

------------------------------------------------------------------------

# 👨‍💻 Autores

**João Teles**
**Adrian Paiva**
**Bruno Prado**

Projeto desenvolvido para a disciplina de **Microcontroladores e
Microprocessadores**.

------------------------------------------------------------------------

# 📄 Licença

Projeto desenvolvido para fins acadêmicos.
