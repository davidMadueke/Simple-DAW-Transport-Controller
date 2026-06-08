#pragma once

template<typename Display>
class DisplayManager {
public:
    void processPendingActions();
    void render(Display& display);
    // postAction(), begin(), etc.
};