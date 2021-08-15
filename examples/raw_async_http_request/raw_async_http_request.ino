#include <WiFi.h>
#include <AsyncTCP.h>

const char* ssid        = "YOUR-WIFI-SSID-HERE";
const char* password    = "YOUR-WIFI-PASSWORD-HERE";

AsyncClient tcpClient;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  while (!Serial);

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  
  Serial.println("Connecting to WiFi SSID: " + String(ssid));

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.print(F("Connected to WiFi @ IP : "));
  Serial.println(WiFi.localIP());

  // Callbacks for data handling, run in separate task to achieve concurrency

  // Callback on successful connection
  tcpClient.onConnect([](void * ctx_ptr, AsyncClient * client) {
    Serial.println("\n\nonConnect successful! sending data...");

    // For the sake of this example, buffer will contain outgoing protocol data
    // not necessarily produced from static ASCII strings.
    char buffer[128];

    // Simulating an HTTP request to http://worldtimeapi.org/api/timezone/Europe/London.txt

    // ASYNC_WRITE_FLAG_COPY is the default value of the apiflags parameter in AsyncTCPSock
    // and can be omitted. Only if the data to be sent is static or long-lived and guaranteed
    // to persist until all data has been written, should you consider passing 0 as apiflags,
    // which will instead store the passed pointer without performing a copy.
    strcpy(buffer, "GET /api/timezone/Europe/London.txt HTTP/1.1\r\n");
    client->add(buffer, strlen(buffer), ASYNC_WRITE_FLAG_COPY);

    strcpy(buffer, "Host: worldtimeapi.org\r\n");
    client->add(buffer, strlen(buffer), ASYNC_WRITE_FLAG_COPY);

    strcpy(buffer, "Connection: close\r\n\r\n");
    client->add(buffer, strlen(buffer), ASYNC_WRITE_FLAG_COPY);

    client->send();
  },
  NULL  // <-- Pointer to application data, accessible within callback through ctx_ptr
  );

  // Callback on data ready to be processed - MUST BE CONSUMED AT ONCE or will be discarded
  tcpClient.onData([](void * ctx_ptr, AsyncClient * client, void * buf, size_t len) {

    Serial.printf("\n\nonData received data (%u bytes), raw buffer follows:\r\n", len);
    Serial.write((const uint8_t *)buf, len);

  },
  NULL  // <-- Pointer to application data, accessible within callback through ctx_ptr
  );

  // Callback on written data being acknowledged as sent. If the data written so far fully covers
  // a buffer added WITHOUT the ASYNC_WRITE_FLAG_COPY flag, now is the first safe moment at
  // which such a buffer area may be discarded or reused.
  tcpClient.onAck([](void * ctx_ptr, AsyncClient * client, size_t len, uint32_t ms_delay) {

    Serial.printf("\n\nonAck acknowledged sending next %u bytes after %u ms\r\n", len, ms_delay);

  },
  NULL  // <-- Pointer to application data, accessible within callback through ctx_ptr
  );

  // Callback on socket disconnect, called:
  // - on any socket close event (local or remote) after being connected
  // - on failure to connect, right after the onError callback
  tcpClient.onDisconnect([](void * ctx_ptr, AsyncClient * client) {

    Serial.println("\n\nonDisconnect socket disconnected!");

  },
  NULL  // <-- Pointer to application data, accessible within callback through ctx_ptr
  );

  // Callback on error event
  tcpClient.onError([](void * ctx_ptr, AsyncClient * client, int8_t error) {

    Serial.printf("\n\nonError socket reported error %d\r\n", error);

  },
  NULL  // <-- Pointer to application data, accessible within callback through ctx_ptr
  );
}

const char * hostname = "worldtimeapi.org";

uint32_t t_dot = 0;
uint32_t t_req = 0;
void loop() {
  uint32_t t = millis();

  // This is to show that the main loop() is running while the AsyncClient
  // object processes data in the background.
  if (t - t_dot >= 200) {
    t_dot = t;
    Serial.print("*");
  }

  // Try connecting to remote host if 10 seconds pass after last try
  if (t - t_req >= 20000 && tcpClient.state() == 0) {

    // NOTE: DNS resolving is also done asynchronously (in the LWIP thread)
    t_req = t;
    Serial.printf("\n\nStarting connection to %s port 80...\r\n", hostname);
    tcpClient.connect(hostname, 80);

  }

  delay(50);
}
