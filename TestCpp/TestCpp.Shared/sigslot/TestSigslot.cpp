#include "TestSigslot.h"
#include "sigslot/Light.h"
#include "sigslot/Switch.h"


TestSigslot::TestSigslot()
{
}


TestSigslot::~TestSigslot()
{
}

void TestSigslot::test()
{
    Light light;
    Switch switch1;

    switch1.mOnClicked.connect(&light, &Light::ToggleState);
}
