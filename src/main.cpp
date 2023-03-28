#include <OV2640.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
 
#include <SimStreamer.h>
#include <OV2640Streamer.h>
#include <CRtspSession.h>
 
// copy this file to wifikeys.h and edit
const char *ssid =     "Xiaomi_E9BA";         // Put your SSID here
const char *password = "1263178881";     // Put your PASSWORD here
 
#define ENABLE_RTSPSERVER
 
OV2640 cam;
 
WiFiServer rtspServer(8554);
 
CStreamer *streamer = nullptr;
CRtspSession *session = nullptr;
WiFiClient client;
void setup()
{
    auto cam_config = esp32cam_aithinker_config;
    cam_config.frame_size = FRAMESIZE_VGA;
    Serial.begin(115200);
    while (!Serial);
    cam.init(cam_config);
 
    IPAddress ip;
 
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(F("."));
    }
    ip = WiFi.localIP();
    Serial.println(F("WiFi connected"));
    Serial.println("");
    Serial.println(ip);
 
    rtspServer.begin();

    //streamer = new SimStreamer(true);             // our streamer for UDP/TCP based RTP transport
    // streamer = new OV2640Streamer(rtspServer,cam);             // our streamer for UDP/TCP based RTP transport
}
 
void loop()
{
    constexpr uint32_t msecPerFrame = 50;
    static uint32_t lastimage = millis();

    // If we have an active client connection, just service that until gone
    // (FIXME - support multiple simultaneous clients)
    if(session) {
        session->handleRequests(0); // we don't use a timeout here,
        // instead we send only if we have new enough frames

        uint32_t now = millis();
        if(now > lastimage + msecPerFrame || now < lastimage) { // handle clock rollover
            session->broadcastCurrentFrame(now);
            lastimage = now;

            // check if we are overrunning our max frame rate
            now = millis();
            if(now > lastimage + msecPerFrame)
                printf("warning exceeding max frame rate of %d ms\n", now - lastimage);
        }

        if(session->m_stopped) {
            delete session;
            delete streamer;
            session = NULL;
            streamer = nullptr;
        }
    }
    else {
        client = rtspServer.accept();
    
        if(client) {
            //streamer = new SimStreamer(&client, true);             // our streamer for UDP/TCP based RTP transport
            streamer = new OV2640Streamer(&client, cam);             // our streamer for UDP/TCP based RTP transport

            session = new CRtspSession(&client, streamer); // our threads RTSP session and state
        }
    }
}