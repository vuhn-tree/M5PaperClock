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
    // TODO: Voltage WiFi Psram touch pos
    _frame_name = "Frame_FactoryTest";

    _canvas_base = new M5EPD_Canvas(&M5.EPD);
    _canvas_data = new M5EPD_Canvas(&M5.EPD);
    _canvas_pos  = new M5EPD_Canvas(&M5.EPD);
    _canvas_btn  = new M5EPD_Canvas(&M5.EPD);
    _canvas_pass = new M5EPD_Canvas(&M5.EPD);

    _canvas_base->createCanvas(300, 600);
    _canvas_data->createCanvas(240, 480);
    _canvas_pos->createCanvas(240, 60);
    _canvas_btn->createCanvas(240, 60);
    _canvas_pass->createCanvas(150, 32);

    _canvas_base->setTextSize(26);
    _canvas_data->setTextSize(26);
    _canvas_pos->setTextSize(26);
    _canvas_btn->setTextSize(26);
    _canvas_pass->setTextSize(26);

    _canvas_base->setTextDatum(CL_DATUM);
    _canvas_data->setTextDatum(CR_DATUM);
    _canvas_pos->setTextDatum(CR_DATUM);
    _canvas_btn->setTextDatum(CR_DATUM);
    _canvas_pass->setTextDatum(CR_DATUM);

    _language = GetLanguage();
    if (_language == LANGUAGE_JA) {
        exitbtn("ホーム");
        _canvas_title->drawString("工場テスト", 270, 34);
    } else if (_language == LANGUAGE_ZH) {
        exitbtn("主页");
        _canvas_title->drawString("出厂测试", 270, 34);
    } else {
        exitbtn("Home");
        _canvas_title->drawString("Factory Test", 270, 34);
    }

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

    _time          = 0;
    _btn           = 0;
    _isfirst       = true;
    _pass_flag     = 0;
    _pass_flag |= GetInitStatus(0) ? 0x0080 : 0;
}

Frame_Clock::~Frame_Clock(void) {
    delete _canvas_base;
    delete _canvas_data;
    delete _canvas_pos;
    delete _canvas_btn;
    delete _canvas_pass;
}

void Frame_Clock::drawItem(uint16_t flag, const char *str, int y) {
    String prefix_pass("[PASS] ");
    String prefix_none("");
    if (_pass_flag & flag) {
        _canvas_base->drawString(prefix_pass + str, POS_LX, y);
    } else {
        _canvas_base->drawString(str, POS_LX, y);
        _canvas_base->ReversePartColor(0, y - 30, 300, 60);
    }
}

void Frame_Clock::drawItem(m5epd_update_mode_t mode) {
    _canvas_base->fillCanvas(0);
    if (_language == LANGUAGE_JA) {
        drawItem(0x0001, "1.期日", 30);
        drawItem(0x0002, "2.時間", 90);
        drawItem(0x0004, "3.温度", 150);
        drawItem(0x0008, "4.湿度", 210);
        drawItem(0x0010, "5.電池", 270);
        drawItem(0x0020, "6.Wi-Fi", 330);
        drawItem(0x0040, "7.PSRAM", 390);
        drawItem(0x0080, "8.SDカード", 450);
        drawItem(0x0100, "9.ボタン", 510);
        drawItem(0x0200, "10.タッチパッド", 570);
    } else if (_language == LANGUAGE_ZH) {
        drawItem(0x0001, "1.日期", 30);
        drawItem(0x0002, "2.时间", 90);
        drawItem(0x0004, "3.温度", 150);
        drawItem(0x0008, "4.湿度", 210);
        drawItem(0x0010, "5.电池", 270);
        drawItem(0x0020, "6.Wi-Fi", 330);
        drawItem(0x0040, "7.PSRAM", 390);
        drawItem(0x0080, "8.SD卡", 450);
        drawItem(0x0100, "9.按键", 510);
        drawItem(0x0200, "10.触屏", 570);
    } else {
        drawItem(0x0001, "1.day", 30);
        drawItem(0x0002, "2.Time", 90);
        drawItem(0x0004, "3.Temperature", 150);
        drawItem(0x0008, "4.Humidity", 210);
    }
    _canvas_base->pushCanvas(0, 100, mode);
}

void Frame_Clock::drawPassCount(m5epd_update_mode_t mode) {
    uint16_t x = _pass_flag, n = 0;
    for (; x; x >>= 1) n += x & 1;
    char buf[20];
    sprintf(buf, "PASS %d/13", n);
    _canvas_pass->fillCanvas(0);
    _canvas_pass->drawString(buf, 150, 14);
    _canvas_pass->pushCanvas(375, 28, mode);
}

int Frame_Clock::run() {
    Frame_Base::run();
    uint16_t pass_flag = _pass_flag;
    char buf[100];
    
    // BTN
    M5.update();
    int ptr        = 0;
    bool ispressed = false;
    if (_btn == 0x07) {
        _btn |= 0x08;
        pass_flag |= 0x0100;
    }
    if (M5.BtnL.isPressed()) {
        _btn |= 0x01;
        buf[ptr++] = 'L';
        ispressed  = true;
    }
    if (M5.BtnP.isPressed()) {
        _btn |= 0x02;
        buf[ptr++] = 'P';
        ispressed  = true;
    }
    if (M5.BtnR.isPressed()) {
        _btn |= 0x04;
        buf[ptr++] = 'R';
        ispressed  = true;
    }
    buf[ptr] = '\0';
    if (ptr == 0) {
        strcpy(buf, "Waiting...");
    }
    if (ispressed) {
        _canvas_btn->fillCanvas(0);
        _canvas_btn->drawString(buf, POS_RX, 30);
        _canvas_btn->pushCanvas(300, 580, UPDATE_MODE_A2);
    }

    if (millis() - _time > 1000) {
        _time = millis();
        rtc_time_t time_struct;
        rtc_date_t date_struct;
        M5.RTC.getTime(&time_struct);
        M5.RTC.getDate(&date_struct);

        if ((date_struct.year > 2010) && (time_struct.hour <= 24) &&
            (time_struct.min <= 60) && (time_struct.sec <= 60)) {
            pass_flag |= 0x01;
        }
        if (_prev_sec == 255) {
            _prev_sec = time_struct.sec;
        }
        if (time_struct.sec != _prev_sec) {
            pass_flag |= 0x02;
        }

        _canvas_data->fillCanvas(0);
        _canvas_data->setTextSize(26);

        // RTC
        sprintf(buf, "%04d - %02d - %02d", date_struct.year, date_struct.mon,
                date_struct.day);
        _canvas_data->drawString(buf, POS_RX, 30);
        sprintf(buf, "%02d : %02d : %02d", time_struct.hour, time_struct.min,
                time_struct.sec);
        _canvas_data->drawString(buf, POS_RX, 90);

        // SHT30
        M5.SHT30.UpdateData();
        if (M5.SHT30.GetError() == 0) {
            float ctemp = M5.SHT30.GetTemperature();
            float chumi = M5.SHT30.GetRelHumidity();

            if (!(pass_flag & 0x04)) {
                if (_prev_temp > 100) {
                    _prev_temp = ctemp;
                }
                if ((ctemp < 40) && (ctemp > 0) && (_prev_temp != ctemp)) {
                    pass_flag |= 0x04;
                }
            }

            if (!(pass_flag & 0x08)) {
                if (_prev_hum > 100) {
                    _prev_hum = chumi;
                }
                if ((chumi >= 0) && (chumi <= 100) && (_prev_hum != chumi)) {
                    pass_flag |= 0x08;
                }
            }

            sprintf(buf, "%.2f ℃", ctemp);
            _canvas_data->drawString(buf, POS_RX, 150);
            sprintf(buf, "%d %%", (int)chumi);
            _canvas_data->drawString(buf, POS_RX, 210);
        } else {
            _canvas_data->drawString("[FAILED]", POS_RX, 150);
            _canvas_data->drawString("[FAILED]", POS_RX, 210);
        }

        _canvas_data->pushCanvas(300, 100, UPDATE_MODE_A2);
    }

    //  grove
    uint16_t temp = pass_flag;
   
    bool update_flag = false;
    if (temp != pass_flag) {
        if (pass_flag != _pass_flag) {
            update_flag = true;
        }
        _pass_flag = pass_flag;
        update_flag = true;
    } else if (update_flag || (pass_flag != _pass_flag)) {
        _pass_flag = pass_flag;
        drawItem(UPDATE_MODE_GL16);
        update_flag = true;
    }
    if (update_flag) {
        drawPassCount(UPDATE_MODE_GL16);
    }

    if (_isfirst) {
        _isfirst = false;
    }

    return 1;
}

int Frame_Clock::init(epdgui_args_vector_t &args) {
    _is_run = 1;
    M5.EPD.Clear();
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    _canvas_base->pushCanvas(0, 100, UPDATE_MODE_NONE);
    drawItem(UPDATE_MODE_NONE);
    drawPassCount(UPDATE_MODE_NONE);
    EPDGUI_AddObject(_key_exit);

    _time          = 0;
    _btn           = 0;
    _isfirst       = true;
    _prev_sec      = 255;
    _prev_temp     = 255;
    _prev_hum      = 255;

    return 3;
}