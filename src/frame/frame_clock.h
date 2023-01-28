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
    uint32_t _time;

};

#endif  //_FRAME_CLOCK_H_