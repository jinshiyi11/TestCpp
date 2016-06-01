#pragma once
#include "sigslot/sigslot.h"

class Light : public sigslot::has_slots<>
{
public:
    void ToggleState();
    void TurnOn();
    void TurnOff();
};
