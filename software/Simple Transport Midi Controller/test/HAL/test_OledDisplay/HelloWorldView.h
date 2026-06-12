#pragma once
#include <U8g2lib.h>

/**
 * HelloWorldView
 *
 * A minimal "view" in the spirit of the pico-ui-demo project
 * (https://github.com/jvanderberg/pico-ui-demo/tree/main/ui-demo):
 * a stateless class exposing a single static `render(Display&)` entry point
 * that draws itself onto the supplied U8g2 driver.
 *
 * It simply paints "Hello World" using the u8g2_font_tenstamps_mf font.
 * The string is split across two lines because the font is too wide to fit
 * "Hello World" on a single 128px row.
 */
template <typename Display>
class HelloWorldView
{
public:
    static void render(Display& display)
    {
        display.setFont(u8g2_font_tenstamps_mf);

        const char* line1 = "Hello";
        const char* line2 = "World";

        display.setFontDirection(0);
        // Horizontally centre each line; drawStr uses the baseline for y.
        display.drawStr((display.getDisplayWidth() - display.getStrWidth(line1)) / 2, 28, line1);
        display.drawStr((display.getDisplayWidth() - display.getStrWidth(line2)) / 2, 58, line2);
    }
};
