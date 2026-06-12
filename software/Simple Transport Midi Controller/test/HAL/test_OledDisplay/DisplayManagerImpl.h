#pragma once
#include <DisplayManager.h>
#include "HelloWorldView.h"
#include "u8G2View.h"

/**
 * Concrete implementation of the DisplayManager<Display> template for the
 * OledDisplay HAL test.
 *
 * DisplayManager.h only declares processPendingActions()/render(); this file
 * supplies their definitions. Keeping the bodies here (in the test folder)
 * lets each application/test decide which views the manager renders without
 * touching the shared HAL/library code.
 *
 * Because these are out-of-line template member definitions, the translation
 * unit that includes this header must also explicitly instantiate the
 * DisplayManager for the concrete driver type (see test_OledDisplay.cpp) so the
 * symbols referenced by OledDisplay.cpp are emitted and resolved at link time.
 */

template <typename Display>
void DisplayManager<Display>::processPendingActions()
{
    // No queued actions for this static "Hello World" screen yet.
}

template <typename Display>
void DisplayManager<Display>::render(Display& display)
{
    static bool toggle = false;
    if (!toggle){ HelloWorldView<Display>::render(display); }
    else { u8g2View<Display>::render(display); }
    vTaskDelay(pdMS_TO_TICKS(3000)); 
    toggle = !toggle;
    

}
