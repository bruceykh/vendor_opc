/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DEVICE_SOC_QUALCOMN_SDM845_HARDWARE_TIMEKEEP_REMINDER_EVENT_MANAGER_H
#define DEVICE_SOC_QUALCOMN_SDM845_HARDWARE_TIMEKEEP_REMINDER_EVENT_MANAGER_H

#include "common_event_subscriber.h"
#include "system_ability_status_change_stub.h"

#include <memory>

#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fstream>

#include <time.h>
#include <sys/time.h>

#define RTC_SYS_FILE "/sys/class/rtc/rtc0/since_epoch"
#define RTC_ATS_FILE "/data/vendor/time/ats_2"

// #define BUNDLE_MGR_SERVICE_SYS_ABILITY_ID 5203

namespace OHOS {
namespace Notification {
class ReminderEventManager {
public:
    explicit ReminderEventManager() {
        init();
    }
    virtual ~ReminderEventManager() {};
    ReminderEventManager(ReminderEventManager &other) = delete;
    ReminderEventManager& operator = (const ReminderEventManager &other) = delete;

private:
    void init() const;

class ReminderEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    ReminderEventSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo): CommonEventSubscriber(subscriberInfo) {}
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data);

    int ReadEpoch(unsigned long long* epoch);
    void RestoreAts(uint64_t value);
    int32_t TimeKeepStore(void);
};

class SystemAbilityStatusChangeListener : public OHOS::SystemAbilityStatusChangeStub {
public:
    explicit SystemAbilityStatusChangeListener() {};
    ~SystemAbilityStatusChangeListener() {};
    virtual void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    virtual void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
};
};
}  // namespace OHOS
}  // namespace Notification
#endif  // DEVICE_SOC_QUALCOMN_SDM845_HARDWARE_TIMEKEEP_REMINDER_EVENT_MANAGER_H
