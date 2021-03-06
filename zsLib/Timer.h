/*

 Copyright (c) 2014, Robin Raymond
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 The views and conclusions contained in the software and documentation are those
 of the authors and should not be interpreted as representing official policies,
 either expressed or implied, of the FreeBSD Project.
 
 */

#pragma once

#ifndef ZSLIB_TIMER_H_625827a73b7747cea09db99f967d6a64
#define ZSLIB_TIMER_H_625827a73b7747cea09db99f967d6a64

#include <zsLib/helpers.h>
#include <zsLib/internal/zsLib_Timer.h>
#include <zsLib/MessageQueueThread.h>

#define ZSLIB_MAX_TIMER_FIRED_AT_ONCE   5

namespace zsLib
{
  typedef Proxy<ITimerDelegate> ITimerDelegateProxy;

  class Timer : public internal::Timer
  {
  protected:
    struct make_private {};

  public:
    Timer(const make_private &, ITimerDelegatePtr delegate, Microseconds timeout, bool repeat, UINT maxFiringTimerAtOnce);

  public:

    static void setMonitorPriority(ThreadPriorities priority);  // must be called before any timer is used

    //-------------------------------------------------------------------------
    // PURPOSE: Create a timer which will fire once or at a repeat interval
    //          until cancelled.
    // NOTES:   The timer is serviced internally by a clock. It's possible
    //          that the clock is put to sleep and wakes up later only to
    //          discover that the timer never fired for many times when it
    //          should have. For example, a computer might be put to sleep
    //          and then the clock is wokeup only to find it is hours later
    //          and missed many events. To fire off hundreds of events at once
    //          is not desirable behaviour so a limit is put which is specified
    //          with "maxFiringTimerAtOnce" to prevent timer wakeup floods.
    static TimerPtr create(
                           ITimerDelegatePtr delegate,
                           Microseconds timeout,
                           bool repeat = true,
                           UINT maxFiringTimerAtOnce = ZSLIB_MAX_TIMER_FIRED_AT_ONCE
                           );

    //-------------------------------------------------------------------------
    // PURPOSE: Helper creation routine for specifying an alternative time unit
    //          other than microseconds
    template <typename TimeUnit>
    static TimerPtr create(
                           ITimerDelegatePtr delegate,
                           TimeUnit timeout,
                           bool repeat = true,
                           UINT maxFiringTimerAtOnce = ZSLIB_MAX_TIMER_FIRED_AT_ONCE
                           ) {
      return create(delegate, std::chrono::duration_cast<Microseconds>(timeout), repeat, maxFiringTimerAtOnce);
    }

    //-------------------------------------------------------------------------
    // PURPOSE: Helper creation routine to timeout at a specific moment in time
    static TimerPtr create(
                           ITimerDelegatePtr delegate,
                           Time timeout
                           );

    ~Timer();

    PUID getID() const {return mID;}
    void cancel();      // cancel a timer (it is no longer needed)

    void background(bool background = true);  // background the timer (will run until timer is cancelled even if reference to object is forgotten)
  };

  interaction ITimerDelegate
  {
    virtual void onTimer(TimerPtr timer) = 0;
  };
}

ZS_DECLARE_PROXY_BEGIN(zsLib::ITimerDelegate)
ZS_DECLARE_PROXY_METHOD_1(onTimer, zsLib::TimerPtr)
ZS_DECLARE_PROXY_END()

#endif //ZSLIB_TIMER_H_625827a73b7747cea09db99f967d6a64
