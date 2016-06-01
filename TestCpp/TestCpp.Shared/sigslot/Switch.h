#pragma once
#include "sigslot/sigslot.h"

class Switch
{
public:
    sigslot::signal0<> mOnClicked;
};
