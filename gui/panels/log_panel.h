#pragma once
#include <string>

class LogPanel {
public:
    bool visible = true;
    void render();
    void clear();
private:
    bool m_autoScroll = true;
};