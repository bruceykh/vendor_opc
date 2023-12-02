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

#include "reminder_event_manager.h"

#include "log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_mgr_interface.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace OHOS::EventFwk;
namespace OHOS {
namespace Notification {

void ReminderEventManager::init() const
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_TIME_CHANGED);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    auto subscriber = std::make_shared<ReminderEventSubscriber>(subscriberInfo);

    std::string identity = IPCSkeleton::ResetCallingIdentity();
    if (CommonEventManager::SubscribeCommonEvent(subscriber)) {
        TIME_LOGD("SubscribeCommonEvent ok");
    } else {
        TIME_LOGD("SubscribeCommonEvent fail");
    }
    IPCSkeleton::SetCallingIdentity(identity);

    sptr<SystemAbilityStatusChangeListener> statusChangeListener
        = new (std::nothrow) SystemAbilityStatusChangeListener();
    if (statusChangeListener == nullptr) {
        TIME_LOGE("Failed to create statusChangeListener due to no memory.");
        return;
    }
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        TIME_LOGD("samgrProxy is null");
        return;
    }
    int32_t ret = samgrProxy->SubscribeSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, statusChangeListener);
    if (ret != ERR_OK) {
        TIME_LOGE("subscribe system ability id: %{public}d failed", BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    }
}

void ReminderEventManager::ReminderEventSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    Want want = data.GetWant();
    std::string action = want.GetAction();
    TIME_LOGI("Recieved common event:%{public}s", action.c_str());
    if (action == CommonEventSupport::COMMON_EVENT_TIME_TICK || 
            action == CommonEventSupport::COMMON_EVENT_TIME_CHANGED || 
            action == CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED) {
        TimeKeepStore();
        return;
    }
}


int ReminderEventManager::ReminderEventSubscriber::ReadEpoch(unsigned long long* epoch) {
	int res = 0;

	int fd = open(RTC_SYS_FILE, O_RDONLY);
	if (fd == -1) {
		res = -errno;
		TIME_LOGE("Failed to open RTC sys path");
	} else {
		char buffer[16];
		memset(buffer, 0, 16);
		res = read(fd, buffer, 16);
		if (res > 0) {
			char *endp = NULL;
			*epoch = strtoull(buffer, &endp, 10);
			// sysfs read returns newline, ok to end up at '\n'
			if (*endp != '\0' && *endp != '\n') {
				TIME_LOGE("Read from " RTC_SYS_FILE " returned "
				      "invalid string %s (%s)", buffer, endp);
				res = -1;
			}
		}
        TIME_LOGD("read RTC sys buffer=%s \n", buffer);
	}

	return res;
}

void ReminderEventManager::ReminderEventSubscriber::RestoreAts(uint64_t value) {
	std::ofstream ofs;
	value *= 1000;
    ofs.open(RTC_ATS_FILE);
    ofs << value;
    ofs.close();
	TIME_LOGD("RestoreAts " RTC_ATS_FILE " value %{public}ld ", value);
}

int32_t ReminderEventManager::ReminderEventSubscriber::TimeKeepStore() {
	unsigned long long seconds = 0;
	unsigned long long epoch_since = 0;
	int res = -1;
	struct tm tm;
	time_t t;

	// Get time and adjust for local time
	time(&t);
	localtime_r(&t, &tm);
	seconds = mktime(&tm);

	if (seconds > 0) {
		res = ReadEpoch(&epoch_since);
		if (res < 0) {
			TIME_LOGE("Failed to read epoch while storing");
		} else {
			seconds -= epoch_since;
			RestoreAts(seconds);
			TIME_LOGD("Time adjustment stored to property seconds=%lld", seconds);
			res = 0;
		}
	}

	return res;
}

void ReminderEventManager::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    TIME_LOGD("OnAddSystemAbilityInner");
}

void ReminderEventManager::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    TIME_LOGD("OnRemoveSystemAbilityInner");
}
}  // namespace OHOS
}  // namespace Notification
