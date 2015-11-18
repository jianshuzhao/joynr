/*
 * #%L
 * %%
 * Copyright (C) 2011 - 2015 BMW Car IT GmbH
 * %%
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * #L%
 */
#ifndef DELAYED_SCHEDULER_H_
#define DELAYED_SCHEDULER_H_

#include "joynr/JoynrCommonExport.h"
#include "joynr/PrivateCopyAssign.h"

#include "joynr/Timer.h"
#include "joynr/Optional.h"

#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace joynr
{
class Runnable;
namespace joynr_logging
{
class Logger;
}

using OptionalDelay = Optional<std::chrono::milliseconds>;

/**
 * @class DelayedScheduler
 * @brief Using a @ref Timer and @ref BlockingQueue to execute a runnable delayed
 */
class JOYNRCOMMON_EXPORT DelayedScheduler
{
public:
    /*! Handle to reference a @ref Runnable scheduled to work */
    typedef Timer::TimerId RunnableHandle;

    /*! Invalid handle */
    static const RunnableHandle INVALID_RUNNABLE_HANDLE = 0;

    /**
     * @brief Constructor
     * @param onWorkAvailable Callback done if work is available after timer
     *      has expired
     * @param defaultDelayMs Default delay used by @ref schedule
     */
    DelayedScheduler(std::function<void(Runnable*)> onWorkAvailable,
                     std::chrono::milliseconds defaultDelayMs = std::chrono::milliseconds::zero());

    /**
     * @brief Destructor
     * @note Be sure to call @ref shutdown and wait for return before
     *      destroying this object
     */
    virtual ~DelayedScheduler();

    /**
     * @brief Schedule a @ref Runnable to be added to execution queue
     * @param runnable Runnable to be added to queue
     * @param delay_ms Number of milliseconds to delay adding the @ref Runnable
     *      to the queue. This parameter is @ref Optional and
     *      if it's value is std::chrono::duration::zero() it will be directly added to the queue.
     *      If this parameter value is invalid (aka null) its default value will be used.
     * @return Handle referencing the given @ref Runnable in this scheduler. If
     *      @ref INVALID_RUNNABLE_HANDLE is returned, the @ref Runnable either
     *      is directly submitted to run or the @ref DelayedScheduler is already
     *      shutting down.
     */
    virtual RunnableHandle schedule(Runnable* runnable,
                                    OptionalDelay optionalDelayMs = OptionalDelay::createNull());

    /**
     * @brief Try to remove a @ref Runnable while waiting
     * @param runnableHandle Handle given by @ref schedule
     */
    virtual void unschedule(const RunnableHandle runnableHandle);

    /**
     * @brief Does an ordinary shutdown of @ref DelayedScheduler
     * @note Must be called before destructor is called
     */
    virtual void shutdown();

private:
    /*! @ref DelayedScheduler is not allowed to be copied */
    DISALLOW_COPY_AND_ASSIGN(DelayedScheduler);

    virtual void timerForRunnableExpired(Timer::TimerId timerId);

    virtual void timerForRunnableRemoved(Timer::TimerId timerId);

private:
    /*! Default delay set by the constructor */
    const std::chrono::milliseconds defaultDelayMs;

    /*! Callback on timer expired */
    std::function<void(Runnable*)> onWorkAvailable;

    /*! Flag indicating @ref DelayedScheduler will be stopped */
    bool stoppingDelayedScheduler;

    /*! Lookup a @ref Runnable from a expiring @ref Timer */
    std::unordered_map<RunnableHandle, Runnable*> timedRunnables;

    /*! Guard to limit write access to @ref timedRunnables */
    std::mutex writeLock;

    /*! Timer to delay added @ref Runnable */
    Timer timer;

    /*! Logger */
    static joynr_logging::Logger* logger;
};

} // namespace joynr
#endif // DELAYED_SCHEDULER_H_