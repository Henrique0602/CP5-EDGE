# Projeto IoT – Monitoramento da Adega da Vinheria Agnello

## 📘 Descrição Geral

Este projeto tem como objetivo realizar o **monitoramento ambiental de uma adega**, pertencente à **Vinheria Agnello**, por meio da captura de dados de **temperatura, umidade e luminosidade** utilizando um **ESP32** integrado a um **sensor DHT11** e um **LDR (Light Dependent Resistor)**.

Os dados coletados são enviados via protocolo **MQTT** para um **broker remoto**, permitindo leitura e análise por meio do aplicativo **MyMQTT**, que funciona como cliente MQTT para visualização dos dados em tempo real.

---

## ⚙️ Componentes Utilizados

| Componente            | Descrição                                                  | Pino                                  |
| --------------------- | ---------------------------------------------------------- | ------------------------------------- |
| **ESP32**             | Microcontrolador responsável pela coleta e envio dos dados | -                                     |
| **Sensor DHT11**      | Mede temperatura (°C) e umidade relativa (%)               | GPIO 4                                |
| **Sensor LDR**        | Mede intensidade luminosa (valor analógico 0–4095)         | GPIO 36 (ADC1_CH0)                    |
| **Wi-Fi**             | Rede utilizada para comunicação MQTT                       | SSID: `FIAP-IOT`, Senha: `F!@p25.IOT` |
| **Broker MQTT**       | Servidor para comunicação entre dispositivos IoT           | IP: `54.227.182.115`, Porta: `1883`   |
| **Aplicativo MyMQTT** | Cliente MQTT para visualização dos dados                   | Android/iOS                           |

---

## 🧠 Conceitos Envolvidos

### 1. **Protocolo MQTT (Message Queuing Telemetry Transport)**

Protocolo leve e eficiente usado em aplicações IoT.

* **Broker**: servidor que gerencia as mensagens entre os dispositivos.
* **Publisher**: dispositivo que envia dados (neste caso, o ESP32).
* **Subscriber**: cliente que recebe dados (MyMQTT).
* **Topic**: canal de comunicação — neste projeto:

  ```
  vinheria/Tropa/Tamandua
  ```

### 2. **Sensoriamento**

* **DHT11** fornece dois parâmetros ambientais:

  * Temperatura (em °C)
  * Umidade relativa do ar (%)
* **LDR** fornece um valor analógico proporcional à luminosidade do ambiente (quanto maior o valor, mais luz incidente).

### 3. **Comunicação Wi-Fi**

O ESP32 conecta-se a uma rede Wi-Fi local e envia os dados via MQTT para o broker remoto.

---

## 🔩 Estrutura do Código

### 1. **Bibliotecas Utilizadas**

```cpp
#include <WiFi.h>          // Conexão Wi-Fi
#include <PubSubClient.h>  // Cliente MQTT
#include <DHT.h>           // Sensor DHT11
```

Essas bibliotecas garantem a comunicação sem fio, a publicação dos dados via MQTT e a leitura dos sensores.

---

### 2. **Definições Iniciais**

```cpp
#define DHTPIN 4
#define DHTTYPE DHT11
#define LDRPIN 36
```

Define os pinos utilizados para o DHT11 e o LDR.

As credenciais e parâmetros de conexão também são definidos:

```cpp
const char* SSID = "FIAP-IOT";
const char* PASSWORD = "F!@p25.IOT";
const char* MQTT_BROKER = "54.227.182.115";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC_TROPA = "vinheria/Tropa/Tamandua";
```

---

### 3. **Setup**

```cpp
void setup() {
  Serial.begin(115200);
  dht.begin();
  connectWifi();
  client.setServer(MQTT_BROKER, MQTT_PORT);
}
```

* Inicializa a comunicação serial para depuração.
* Inicializa o DHT11.
* Conecta o ESP32 à rede Wi-Fi.
* Define o broker MQTT que será utilizado.

---

### 4. **Loop Principal**

```cpp
void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  delay(10000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int l = analogRead(LDRPIN);
```

* Garante que o cliente MQTT esteja conectado.
* Realiza a leitura dos sensores a cada **10 segundos**.
* Se os valores forem válidos, são formatados em JSON e publicados no broker:

```cpp
String jsonPayload = "{";
jsonPayload += "\"temp\": " + String(t) + ",";
jsonPayload += "\"hum\": " + String(h) + ",";
jsonPayload += "\"light\": " + String(l);
jsonPayload += "}";
client.publish(MQTT_TOPIC_TROPA, jsonPayload.c_str());
```

Formato do **payload MQTT**:

```json
{
  "temp": 22.3,
  "hum": 58.4,
  "light": 1750
}
```

---

### 5. **Função de Conexão Wi-Fi**

```cpp
void connectWifi() {
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("WiFi conectado!");
}
```

Garante que o ESP32 permaneça em loop até a conexão ser estabelecida.

---

### 6. **Função de Reconexão MQTT**

```cpp
void reconnectMQTT() {
  while (!client.connected()) {
    if (client.connect("ESP32_Vinheria")) {
      Serial.println("conectado!");
    } else {
      delay(5000);
    }
  }
}
```

Mantém a persistência da conexão MQTT, tentando reconectar em caso de falha.

---

## 📲 Configuração do Aplicativo MyMQTT

1. **Instalar o app**: [MyMQTT (Play Store)](https://play.google.com/store/apps/details?id=at.tripwire.mqtt.client)
2. **Adicionar nova conexão:**

   * **Broker Address:** `54.227.182.115`
   * **Port:** `1883`
   * **Client ID:** `MyMQTT_Vinheria`
3. **Assinar o tópico:**

   ```
   vinheria/Tropa/Tamandua
   ```
4. **Visualizar os dados JSON** sendo recebidos a cada 10 segundos.

---

## 🧪 Testes e Validação

### Testes realizados:

* **Verificação de conexão Wi-Fi:** Confirmar IP no monitor serial.
* **Leitura dos sensores:** Exibir valores no monitor serial.
* **Publicação MQTT:** Confirmar recebimento no aplicativo MyMQTT.
* **Validação do JSON:** Conferir formato e conteúdo no tópico.

### Resultados esperados:

* Conexão estável com o Wi-Fi.
* Leitura válida de temperatura, umidade e luminosidade.
* Envio contínuo de dados MQTT a cada 10 segundos.

---

## 🧾 Conclusão

Este projeto implementa com sucesso um **sistema IoT de monitoramento ambiental** utilizando **ESP32, DHT11, LDR e MQTT**, possibilitando o acompanhamento em tempo real das condições da adega da Vinheria Agnello.
A comunicação é confiável, os dados são estruturados em JSON e a arquitetura pode ser facilmente expandida para aplicações industriais e domésticas.

---

**Grupo:** [Eric Koigim, 563290 | Henrique Brigo, 563323 | Guilherme Guimarães, 564226 | Pedro Gomes, 563191 | Piettra Boschini, 562538 | Raul Laguna, 562684]
**Disciplina:** Edge Computing And Computer System
