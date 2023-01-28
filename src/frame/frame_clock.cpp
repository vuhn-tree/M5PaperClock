#include "frame_clock.h"
#include "WiFi.h"

#define POS_LX (15)
#define POS_RX (240 - 15)

const uint8_t kPins[6] = {32, 25, 33, 26, 19, 18};

void frame_clock_exit_cb(epdgui_args_vector_t &args) {
    EPDGUI_PopFrame();
    *((int *)(args[0])) = 0;
}

Frame_Clock::Frame_Clock(void) {
    _frame_name = "Frame_Clock";

    _canvas_data = new M5EPD_Canvas(&M5.EPD);
    _canvas_data->createCanvas(240, 480);
    _canvas_data->setTextSize(26);
    _canvas_data->setTextDatum(CR_DATUM);

    exitbtn("Home");
    _canvas_title->drawString("Clock", 270, 34);

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void *)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);

    for (int i = 0; i < 6; i++) {
        pinMode(kPins[i], OUTPUT);
        digitalWrite(kPins[i], HIGH);
    }
    M5.SHT30.Begin();
    M5.RTC.begin();

    rtc_time_t time_struct;
    rtc_date_t date_struct;
    M5.RTC.getTime(&time_struct);
    M5.RTC.getDate(&date_struct);

    if ((isTimeSynced() == 0) || (date_struct.year < 2010)) {
        rtc_time_t time_struct;
        time_struct.hour = 23;
        time_struct.min  = 33;
        time_struct.sec  = 33;
        M5.RTC.setTime(&time_struct);
        rtc_date_t date_struct;
        date_struct.week = 4;
        date_struct.mon  = 9;
        date_struct.day  = 24;
        date_struct.year = 2020;
        M5.RTC.setDate(&date_struct);
    }

    _time = 0;
    
}

Frame_Clock::~Frame_Clock(void) {
    delete _canvas_data;
}

int Frame_Clock::run() {
    Frame_Base::run();

    char buf[100];

    if (millis() - _time > 1000) {
        _time = millis();
        rtc_time_t time_struct;
        rtc_date_t date_struct;
        M5.RTC.getTime(&time_struct);
        M5.RTC.getDate(&date_struct);
        
        _canvas_data->fillCanvas(0);
        _canvas_data->setTextSize(26);

        // RTC Date
        sprintf(buf, "%04d - %02d - %02d", date_struct.year, date_struct.mon,
                date_struct.day);
        _canvas_data->drawString(buf, POS_RX, 30);

        // RTC Time
        sprintf(buf, "%02d : %02d : %02d", time_struct.hour, time_struct.min,
                time_struct.sec);
        _canvas_data->drawString(buf, POS_RX, 90);

        // SHT30
        M5.SHT30.UpdateData();
        if (M5.SHT30.GetError() == 0) {
            float ctemp = M5.SHT30.GetTemperature(M5.SHT30.TemperatureScale::Fah);
            float chumi = M5.SHT30.GetRelHumidity();

            // temperature
            sprintf(buf, "%.2f F", ctemp);
            _canvas_data->drawString(buf, POS_RX, 150);

            // humidity
            sprintf(buf, "%d %%", (int)chumi);
            _canvas_data->drawString(buf, POS_RX, 210);
        } else {
            _canvas_data->drawString("[FAILED]", POS_RX, 150);
            _canvas_data->drawString("[FAILED]", POS_RX, 210);
        }

        _canvas_data->pushCanvas(300, 100, UPDATE_MODE_A2);
    }
    return 1;
}

int Frame_Clock::init(epdgui_args_vector_t &args) {
    _is_run = 1;
    M5.EPD.Clear();
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_key_exit);
    _time = 0;
    return 3;
}