#include <Arduino.h>

#ifdef ESP8266
#include "Hash.h"
#include <ESPAsyncTCP.h>
#else
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include "painlessMesh.h"

//INCLUIR AS BIBLIOTECAS NECESSARIAS PARA OS SENSORES A SEREM USADOS
//
//

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555
#define ID 16

// Configurar a rede WIFI
#define STATION_SSID "SSID da rede WIFI"
#define STATION_PASSWORD "senha da rede wifi"
#define STATION_CHANNEL 3

//INCLUIR AS CONFIGURAÇÕES DE GPIO E VARIAVEIS PARA OS SENSORES USADOS
//
//

//$$$$$$$$$$$$$$$$$$$$$
// WATCHDOG
hw_timer_t *timer = NULL; // faz o controle do temporizador (interrupção por tempo)

// função que o temporizador irá chamar, para reiniciar o ESP32
void IRAM_ATTR resetModule()
{
  ets_printf("(watchdog) reiniciar\n"); // imprime no log
  esp_restart();                        // reinicia o chip
  // esp_restart_noos(); //não funcionou foi substituída pela linha de cima.
}
//$$$$$$$$$$$$$$$$$$$$$

//================================
// Configuracao REDE MESH
byte contCheckConnection = 0;
byte contCheckConnectionRoot = 0;

Scheduler userScheduler;
painlessMesh mesh;

// Prototype
void receivedCallback(uint32_t from, String &msg);
void getMessageHello(JsonDocument myObject);
void sendMessageHello();
void checkConnection();

Task taskSendMessageHello(TASK_SECOND * 11, TASK_FOREVER, &sendMessageHello);
Task taskCheckConnection(TASK_SECOND * 20, TASK_FOREVER, &checkConnection);
//ADICIONAR TASK DE LEITURA DOS SENSORES
//
//

//ADICIONAR FUNÇÕES PARA LEITURA DOS SENSORES USADOS
//
//

void sendMessageHello()
{
  JsonDocument hello;
  hello["code"] = 100;
  hello["id"] = ID;
  hello["description"] = "Modulo Caixa Dagua";

  String json;
  serializeJson(hello, json);
  mesh.sendBroadcast(json);
  Serial.print("Send Message Hello = ");
  Serial.println(json);
}

void getMessageHello(JsonDocument doc)
{
  byte id = doc["id"];

  Serial.print("ID recebido = ");
  Serial.println(id);
}

void checkConnection()
{
  // Se a Lista de Node igual a 01 reinicia a rede mesh para tentar se reconectar
  contCheckConnection++;
  Serial.print("NodeList = ");
  Serial.println(mesh.getNodeList(true).size());
  Serial.print("total de verificações de conexão ativa: ");
  Serial.println(contCheckConnection);

  if (mesh.getNodeList(true).size() == 1)
  {
    if (contCheckConnection > 7)
    {
      Serial.println(" ###### ==== ##### REINICIAR ESP32 ******&&&&&******&&&&");
      esp_restart(); // reinicia o chip
    }
  }
  else
  {
    contCheckConnection = 0;
  }
}

void receivedCallback(uint32_t from, String &msg)
{
  Serial.printf("startHere: Received from %u msg= %s\n", from, msg.c_str());
  Serial.println();

  JsonDocument doc;
  deserializeJson(doc, msg.c_str());

  byte code = doc["code"];

  switch (code)
  {
  case 100:
    getMessageHello(doc);
    break;

  case 200:
    break;

  default:
    break;
  }
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.println();
}

void changedConnectionCallback()
{
  Serial.printf("Changed connections\n");
  Serial.println();
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
  Serial.println();
}
//================================

void setup()
{
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);

  // Configurar o canal do AP, deverá ser o mesmo para toda a rede mesh e roteador wifi
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, STATION_CHANNEL);

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessageHello); 
  userScheduler.addTask(taskCheckConnection);
  
  //ADICIONAR TASK DE LEITURA DOS SENSORES E HABILITAR
  //
  //

  taskSendMessageHello.enable(); 
  taskCheckConnection.enable();

  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);

  // WATCHDOG
  // hw_timer_t * timerBegin(uint8_t num, uint16_t divider, bool countUp)
  /*
     num: é a ordem do temporizador. Podemos ter quatro temporizadores, então a ordem pode ser [0,1,2,3].
    divider: É um prescaler (reduz a frequencia por fator). Para fazer um agendador de um segundo,
    usaremos o divider como 80 (clock principal do ESP32 é 80MHz). Cada instante será T = 1/(80) = 1us
    countUp: True o contador será progressivo
  */
  timer = timerBegin(0, 80, true); // timerID 0, div 80
  // timer, callback, interrupção de borda
  timerAttachInterrupt(timer, &resetModule, true);
  // timer, tempo (us), repetição
  timerAlarmWrite(timer, 10000000, true); // reinicia após 10s
  timerAlarmEnable(timer);                // habilita a interrupção

}

void loop()
{
  mesh.update();

  // WATCHDOGs
  timerWrite(timer, 0); // reseta o temporizador (alimenta o watchdog)

  //ADICIONAR CONFIGURAÇÕES NECESSÁRIAS PARA FUNCIONAMENTO DOS SENSORES
  //
  //
}
