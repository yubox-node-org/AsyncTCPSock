#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>

const char* ssid        = "YOUR-WIFI-SSID-HERE";
const char* password    = "YOUR-WIFI-PASSWORD-HERE";

// On your Arduino projects, create the file build_opt.h if it does not exist
// and add the following single line to enable AsyncTCP SSL support:
// -DASYNC_TCP_SSL_ENABLED=1
#ifndef ASYNC_TCP_SSL_ENABLED
#error The macro ASYNC_TCP_SSL_ENABLED has not been correctly enabled in your environment!
#endif

// To extract root certificate for test:
// openssl s_client -showcerts -connect SSLDOMAIN.ORG:443 < /dev/null

// Root certificate test site (Let's Encrypt)

const char * hostname = "www.eff.org";

const char * rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFYDCCBEigAwIBAgIQQAF3ITfU6UK47naqPGQKtzANBgkqhkiG9w0BAQsFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTIxMDEyMDE5MTQwM1oXDTI0MDkzMDE4MTQwM1ow\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwggIiMA0GCSqGSIb3DQEB\n" \
"AQUAA4ICDwAwggIKAoICAQCt6CRz9BQ385ueK1coHIe+3LffOJCMbjzmV6B493XC\n" \
"ov71am72AE8o295ohmxEk7axY/0UEmu/H9LqMZshftEzPLpI9d1537O4/xLxIZpL\n" \
"wYqGcWlKZmZsj348cL+tKSIG8+TA5oCu4kuPt5l+lAOf00eXfJlII1PoOK5PCm+D\n" \
"LtFJV4yAdLbaL9A4jXsDcCEbdfIwPPqPrt3aY6vrFk/CjhFLfs8L6P+1dy70sntK\n" \
"4EwSJQxwjQMpoOFTJOwT2e4ZvxCzSow/iaNhUd6shweU9GNx7C7ib1uYgeGJXDR5\n" \
"bHbvO5BieebbpJovJsXQEOEO3tkQjhb7t/eo98flAgeYjzYIlefiN5YNNnWe+w5y\n" \
"sR2bvAP5SQXYgd0FtCrWQemsAXaVCg/Y39W9Eh81LygXbNKYwagJZHduRze6zqxZ\n" \
"Xmidf3LWicUGQSk+WT7dJvUkyRGnWqNMQB9GoZm1pzpRboY7nn1ypxIFeFntPlF4\n" \
"FQsDj43QLwWyPntKHEtzBRL8xurgUBN8Q5N0s8p0544fAQjQMNRbcTa0B7rBMDBc\n" \
"SLeCO5imfWCKoqMpgsy6vYMEG6KDA0Gh1gXxG8K28Kh8hjtGqEgqiNx2mna/H2ql\n" \
"PRmP6zjzZN7IKw0KKP/32+IVQtQi0Cdd4Xn+GOdwiK1O5tmLOsbdJ1Fu/7xk9TND\n" \
"TwIDAQABo4IBRjCCAUIwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYw\n" \
"SwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5pZGVudHJ1\n" \
"c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTEp7Gkeyxx\n" \
"+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEEAYLfEwEB\n" \
"ATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2VuY3J5cHQu\n" \
"b3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0LmNvbS9E\n" \
"U1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFHm0WeZ7tuXkAXOACIjIGlj26Ztu\n" \
"MA0GCSqGSIb3DQEBCwUAA4IBAQAKcwBslm7/DlLQrt2M51oGrS+o44+/yQoDFVDC\n" \
"5WxCu2+b9LRPwkSICHXM6webFGJueN7sJ7o5XPWioW5WlHAQU7G75K/QosMrAdSW\n" \
"9MUgNTP52GE24HGNtLi1qoJFlcDyqSMo59ahy2cI2qBDLKobkx/J3vWraV0T9VuG\n" \
"WCLKTVXkcGdtwlfFRjlBz4pYg1htmf5X6DYO8A4jqv2Il9DjXA6USbW1FzXSLr9O\n" \
"he8Y4IWS6wY7bCkjCWDcRQJMEhg76fsO3txE+FiYruq9RUWhiF1myv4Q6W+CyBFC\n" \
"Dfvp7OOGAN6dEOM4+qR9sdjoSYKEBpsr6GtPAQw4dy753ec5\n" \
"-----END CERTIFICATE-----\n";

AsyncClient sslClient;

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
  sslClient.onConnect([](void * ctx_ptr, AsyncClient * client) {
    Serial.println("\n\nonConnect successful! sending data...");

    // For the sake of this example, buffer will contain outgoing protocol data
    // not necessarily produced from static ASCII strings.
    char buffer[128];

    // Simulating an HTTP request

    // ASYNC_WRITE_FLAG_COPY is the default value of the apiflags parameter in AsyncTCPSock
    // and can be omitted. Only if the data to be sent is static or long-lived and guaranteed
    // to persist until all data has been written, should you consider passing 0 as apiflags,
    // which will instead store the passed pointer without performing a copy.
    strcpy(buffer, "GET / HTTP/1.1\r\n");
    client->add(buffer, strlen(buffer), ASYNC_WRITE_FLAG_COPY);

    snprintf(buffer, sizeof(buffer), "Host: %s\r\n", hostname);
    client->add(buffer, strlen(buffer), ASYNC_WRITE_FLAG_COPY);

    strcpy(buffer, "Connection: close\r\n\r\n");
    client->add(buffer, strlen(buffer), ASYNC_WRITE_FLAG_COPY);

    client->send();
  },
  NULL  // <-- Pointer to application data, accessible within callback through ctx_ptr
  );

  // Callback on data ready to be processed - MUST BE CONSUMED AT ONCE or will be discarded
  sslClient.onData([](void * ctx_ptr, AsyncClient * client, void * buf, size_t len) {

    Serial.printf("\n\nonData received data (%u bytes), raw buffer follows:\r\n", len);
    Serial.write((const uint8_t *)buf, len);

  },
  NULL  // <-- Pointer to application data, accessible within callback through ctx_ptr
  );

  // Callback on written data being acknowledged as sent. If the data written so far fully covers
  // a buffer added WITHOUT the ASYNC_WRITE_FLAG_COPY flag, now is the first safe moment at
  // which such a buffer area may be discarded or reused.
  sslClient.onAck([](void * ctx_ptr, AsyncClient * client, size_t len, uint32_t ms_delay) {

    Serial.printf("\n\nonAck acknowledged sending next %u bytes after %u ms\r\n", len, ms_delay);

  },
  NULL  // <-- Pointer to application data, accessible within callback through ctx_ptr
  );

  // Callback on socket disconnect, called:
  // - on any socket close event (local or remote) after being connected
  // - on failure to connect, right after the onError callback
  sslClient.onDisconnect([](void * ctx_ptr, AsyncClient * client) {

    Serial.println("\n\nonDisconnect socket disconnected!");

  },
  NULL  // <-- Pointer to application data, accessible within callback through ctx_ptr
  );

  // Callback on error event
  sslClient.onError([](void * ctx_ptr, AsyncClient * client, int8_t error) {

    Serial.printf("\n\nonError socket reported error %d\r\n", error);

  },
  NULL  // <-- Pointer to application data, accessible within callback through ctx_ptr
  );

  sslClient.setRootCa(rootCACertificate, strlen(rootCACertificate) + 1);
  
}

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
  if (t - t_req >= 20000 && sslClient.state() == 0) {

    // NOTE: DNS resolving is also done asynchronously (in the LWIP thread)
    t_req = t;
    Serial.printf("\n\nStarting connection to %s port 443...\r\n", hostname);
    sslClient.connect(hostname, 443, true); // <-- Enable encrypted connection on this instance

  }

  delay(50);
}
