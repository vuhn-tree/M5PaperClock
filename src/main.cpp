// #include <M5EPD.h>
// #include "epdgui/epdgui.h"
// #include "frame/frame.h"
// #include "systeminit.h"

// void setup() {
//     pinMode(M5EPD_MAIN_PWR_PIN, OUTPUT);
//     M5.enableMainPower();

//     SysInit_Start();
// }

// void loop() {
//     EPDGUI_MainLoop();
// }

#include <M5EPD.h>

M5EPD_Canvas canvas(&M5.EPD);
/* After M5Paper is started or reset
the program in the setUp () function will be run, and this part will only be run once.
*/
void setup(){
    M5.begin();   
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);  //Clear the screen.  
    M5.RTC.begin();  //Init the RTC.
    canvas.createCanvas(540, 960);  
    canvas.setTextSize(3); 
    canvas.drawString("Hello World2", 45, 350);  
    canvas.pushCanvas(0,0,UPDATE_MODE_DU4);  //Update the screen.
}
/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly */
void loop(){

}