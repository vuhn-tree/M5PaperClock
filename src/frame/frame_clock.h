#ifndef _FRAME_CLOCK_H_
#define _FRAME_CLOCK_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"

class Frame_Clock : public Frame_Base {
   public:
    Frame_Clock();
    ~Frame_Clock();
    int run();
    int init(epdgui_args_vector_t &args);

   private:
    void drawItem(m5epd_update_mode_t mode);
    void drawItem(uint16_t flag, const char *str, int y);

   private:
    M5EPD_Canvas *_canvas_base;
    M5EPD_Canvas *_canvas_data;
    M5EPD_Canvas *_canvas_pos;
    M5EPD_Canvas *_canvas_btn;
    M5EPD_Canvas *_canvas_pass;
    uint32_t _time;
    uint8_t _btn;
    uint8_t _prev_sec;
    float _prev_temp;
    float _prev_hum;
};

#endif  //_FRAME_CLOCK_H_