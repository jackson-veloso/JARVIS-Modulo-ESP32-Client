
# JARVIS MODULO ESP32 CLIENT

Código base para implementar um nó na rede mesh usando módulos ESP32 e ESP01.  
Código para placas ESP32

## CONFIGURAÇÕES
- ID: ID do módulo no projeto JARVIS  
- STATION_SSID: nome da rede wifi  
- STATION_PASSWORD: senha da rede wifi  
- STATION_CHANNEL: canal do AP (necessário configurar o AP local para canal fixo)  

## FUNÇÕES
getMessageHello(JsonDocument myObject); imprime as msg hello recebidas  
sendMessageHello(); envia msg hello a cada 11s  
checkConnection(); reinicia o ESP se perder conexao a rede mesh  

## Observações:
- Somente um dispositivo pode ser o nó raiz na malha (root)  
- Configurar o SSID e Senha da Rede WIFI Local
- Configurar Roteador WIFI para canal Manual, o mesmo canal do AP deve ser configurado em todos os dispositivos pertencentes a rede mesh

## Referências
https://github.com/jackson-veloso/ESP32_WATCHDOG
https://github.com/jackson-veloso/ESP32_REDE_MESH_PAINLESSMESH

https://blog.eletrogate.com/criando-uma-rede-mesh-com-os-modulos-esp-8266-esp-32-esp-01/  
https://randomnerdtutorials.com/esp-mesh-esp32-esp8266-painlessmesh/  
https://gitlab.com/painlessMesh/painlessMesh  
https://www.fernandok.com/2018/06/travou-e-agora.html  
https://labdegaragem.com/forum/topics/esp32-travou-e-agora  
