#pragma once

#include <remotery/Remotery.h>

/*
Basic use:

void SomeClass::someSlowFunction()
{
    rmt_ScopedCPUSample(SomeClass_someSlowFunction, 0);

    // Use RMTSF_Aggregate to sum up many sibling scopes:

    for (...)
    {
        {
            rmt_ScopedCPUSample(load, RMTSF_Aggregate);
            load(i);
        }
        {
            rmt_ScopedCPUSample(save, RMTSF_Aggregate);
            save(i);
        }
    }
}

Start the app with --profile and open komb/thirdparty/Remotery/vis/index.html in a browser.
If Remotery can't connect to your app, restart the app and try again.

That's it!
*/

namespace komb {

/// called by initLogging if --profile
bool startProfilerServer();

} // namespace komb
