#define test

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

M5EPD_Canvas canvas(&M5.EPD);
/* After M5Paper is started or reset
the program in the setUp () function will be run, and this part will only be run once.
*/
void setup()
{
    M5.begin();
    M5.EPD.SetRotation(0);
    M5.EPD.Clear(true); // Clear the screen.
    M5.RTC.begin();     // Init the RTC.
    // canvas.createCanvas(540, 960);
    canvas.createCanvas(960, 540);
    canvas.setTextSize(3);
    // canvas.drawString("Hello World2", 45, 350);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4); // Update the screen.
}
/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly */
void loop()
{

    char buf[130];
    // buf[ptr] = '\0';
    // if (ptr == 0) {
    //     strcpy(buf, "Waiting...");
    // }
    // _time = millis();
    rtc_time_t time_struct;
    rtc_date_t date_struct;
    M5.RTC.getTime(&time_struct);
    M5.RTC.getDate(&date_struct);

    if ((date_struct.year > 2010) && (time_struct.hour <= 24) &&
        (time_struct.min <= 60) && (time_struct.sec <= 60))
    {
        // pass_flag |= 0x01;
    }
    // if (_prev_sec == 255) {
    //     _prev_sec = time_struct.sec;
    // }
    // if (time_struct.sec != _prev_sec) {
    //     pass_flag |= 0x02;
    // }

    // _canvas_data->fillCanvas(0);
    // _canvas_data->setTextSize(26);

    canvas.fillCanvas(0);
    canvas.setTextSize(26);

    // RTC
    sprintf(buf, "%04d - %02d - %02d", date_struct.year, date_struct.mon,
            date_struct.day);
    canvas.drawString(buf, 30, 300);
    sprintf(buf, "%02d : %02d : %02d", time_struct.hour, time_struct.min,
            time_struct.sec);
    canvas.drawString(buf, 50, 400);

    canvas.pushCanvas(0, 0, UPDATE_MODE_A2);
}

#endif