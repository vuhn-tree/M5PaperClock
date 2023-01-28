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

int _time = 0;
M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    M5.begin();
    M5.EPD.SetRotation(180);
    M5.EPD.Clear(true); // Clear the screen.
    M5.RTC.begin();     // Init the RTC.

    canvas.createCanvas(960, 540);
    canvas.setTextSize(20);
    canvas.fillCanvas(0);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4); // Update the screen.
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
    // delay(100);

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
        canvas.drawString(buf, 30, 300);

        // RTC Time
        sprintf(buf, "%02d : %02d : %02d", time_struct.hour, time_struct.min,
                time_struct.sec);
        canvas.drawString(buf, 50, 400);

        canvas.pushCanvas(0, 0, UPDATE_MODE_A2);
    }
}

#endif