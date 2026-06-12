#pragma once
#include <U8g2lib.h>

template <typename Display>
class u8g2View
{
public:
    static void render(Display& display)
    {
         display.setFontMode(1);	// Transparent

        display.setFontDirection(0);
        display.setFont(u8g2_font_inb16_mf);
        display.drawStr(0, 22, "U");
        
        display.setFontDirection(1);
        display.setFont(u8g2_font_inb19_mn);
        display.drawStr(14,8,"8");
        
        display.setFontDirection(0);
        display.setFont(u8g2_font_inb16_mf);
        display.drawStr(36,22,"g");
        display.drawStr(48,22,"\xb2");
        
        display.drawHLine(2, 25, 34);
        display.drawHLine(3, 26, 34);
        display.drawVLine(32, 22, 12);
        display.drawVLine(33, 23, 12);



        display.setFont(u8g2_font_4x6_tr);
        if ( display.getDisplayHeight() < 59 )
        {
            display.drawStr(89,20,"github.com");
            display.drawStr(73,29,"/olikraus/u8g2");
        }
        else
        {
            display.drawStr(1,54,"github.com/olikraus/u8g2");
        }
    }
};