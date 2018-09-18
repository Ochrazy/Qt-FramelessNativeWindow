#ifdef __APPLE__
#include "Clicker.h"

#include <Foundation/NSProcessInfo.h>

static id m_activity = nil;

void Clicker::suspendAppNap()
{
    // Begin NSActivityBackground to prevent App Napping
    m_activity = [[NSProcessInfo processInfo]
                      beginActivityWithOptions:NSActivityBackground
                                        reason:@"MyReason"];
    [m_activity retain];

}

void Clicker::resumeAppNap()
{
    // End NSActivityBackground when done
    [[NSProcessInfo processInfo] endActivity:m_activity];
    [m_activity release];
}

#endif
