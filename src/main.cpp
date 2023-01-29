// #define test

#ifdef test
#include <M5EPD.h>
#include "epdgui/epdgui.h"
#include "frame/frame.h"
#include "systeminit.h"

void setup()
{
    pinMode(M5EPD_MAIN_PWR_PIN, OUTPUT);
    M5.enableMainPower();

    SysInit_Start();
}

void loop()
{
    EPDGUI_MainLoop();
}
#else

#define POS_LX (15)
#define POS_RX (240 - 15)

#include <M5EPD.h>
#include "resources/binaryttf.h"
#include "WiFi.h"

void scan(String *ssid, int32_t *rssi);

int _time = 0;
M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    M5.begin();
    M5.EPD.SetRotation(180);
    M5.EPD.Clear(true); // Clear the screen.
    M5.RTC.begin();     // Init the RTC.

    char buf[100];
    String ssid;
    int32_t rssi;
    scan(&ssid, &rssi);
    sprintf(buf, "%s (%d db)", ssid.c_str(), rssi);
    // log_d("")

    canvas.createCanvas(960, 540);
    // canvas.loadFont(binaryttf, sizeof(binaryttf));
    // canvas.createRender(36, 14);
    canvas.setTextSize(88);
    canvas.fillCanvas(0);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4); // Update the screen.
    
    canvas.drawString(buf, 50, 50);
}

void loop()
{
    if (M5.BtnL.wasPressed()) {
        M5.EPD.Active();
    }
    if (M5.BtnP.wasPressed()) {
        delay(1000);
        M5.EPD.StandBy();
    }
    if (M5.BtnR.wasPressed()) {
        delay(1000);
        M5.EPD.Sleep();
    }
    M5.BtnL.lastChange();
    M5.update();
    delay(100);

    if (millis() - _time > 1000)
    {
        _time = millis();
        char buf[130];
        rtc_time_t time_struct;
        rtc_date_t date_struct;
        M5.RTC.getTime(&time_struct);
        M5.RTC.getDate(&date_struct);

        // RTC Date
        sprintf(buf, "%04d - %02d - %02d", date_struct.year, date_struct.mon,
                date_struct.day);
        int centerX = (960 / 2) - (canvas.textWidth(buf) / 2);
        canvas.drawString(buf, centerX, 10);

        // RTC Time
        sprintf(buf, "%02d : %02d : %02d", time_struct.hour, time_struct.min,
                time_struct.sec);
        centerX = (960 / 2) - (canvas.textWidth(buf) / 2);
        int centerY = (540 / 2) - (canvas.fontHeight() / 2);
        canvas.drawString(buf, centerX, centerY);

        canvas.pushCanvas(0, 0, UPDATE_MODE_A2);
    }
}

void scan(String *ssid, int32_t *rssi) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.scanNetworks(true);

    int wifi_num;
    while (1) {
        wifi_num = WiFi.scanComplete();
        if (wifi_num >= 0) {
            break;
        }
    }

    *ssid = WiFi.SSID(0);
    *rssi = WiFi.RSSI(0);
    WiFi.scanDelete();
}

#endif