/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "qpower_service_ability.h"
#include "log_wrapper.h"
#include <unistd.h>
#include "hdf_io_service_if.h"

using namespace OHOS::HDI::Battery;

namespace OHOS {
namespace PowerMgr {
namespace {
REGISTER_SYSTEM_ABILITY_BY_ID(QPowerServiceAbility, QPOWER_SERVICE_ABILITY_ID, true);
constexpr const char* BATTERY_HDI_NAME = "battery_interface_service";
constexpr uint32_t RETRY_TIME = 1000;
}

QPowerServiceAbility::QPowerServiceAbility(const int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate) {}

QPowerServiceAbility::~QPowerServiceAbility()
{}

void QPowerServiceAbility::OnStart()
{
    QPOWER_LOGD("OnStart");
    //RegisterBatteryHdiCallback();
    RegisterHdiStatusListener();
}

void QPowerServiceAbility::OnStop()
{
    QPOWER_LOGD("OnStop");
}

bool QPowerServiceAbility::RegisterHdiStatusListener()
{
    hdiServiceMgr_ = OHOS::HDI::ServiceManager::V1_0::IServiceManager::Get();
    if (hdiServiceMgr_ == nullptr) {
        QPOWER_LOGE("hdi service manager is nullptr");
        usleep(RETRY_TIME);
        RegisterHdiStatusListener();
        return false;
    }

    hdiServStatListener_ = new HdiServiceStatusListener(HdiServiceStatusListener::StatusCallback(
        [&](const OHOS::HDI::ServiceManager::V1_0::ServiceStatus &status) {
            if (status.serviceName != BATTERY_HDI_NAME || status.deviceClass != DEVICE_CLASS_DEFAULT) {
                return;
            }

            if (status.status == SERVIE_STATUS_START) {
                //SendEvent(BatteryServiceEventHandler::EVENT_REGISTER_BATTERY_HDI_CALLBACK, 0);
                QPOWER_LOGI("battery interface service start");
                RegisterBatteryHdiCallback();
            } else if (status.status == SERVIE_STATUS_STOP && iBatteryInterface_) {
                iBatteryInterface_->UnRegister();
                iBatteryInterface_ = nullptr;
                QPOWER_LOGW("battery interface service stop, unregister interface");
            }
        }
    ));

    int32_t status = hdiServiceMgr_->RegisterServiceStatusListener(hdiServStatListener_, DEVICE_CLASS_DEFAULT);
    if (status != ERR_OK) {
        QPOWER_LOGE("Register hdi failed");
        usleep(RETRY_TIME);
        RegisterHdiStatusListener();
        return false;
    }
    return true;
}

bool QPowerServiceAbility::RegisterBatteryHdiCallback()
{
    if (iBatteryInterface_ == nullptr) {
        iBatteryInterface_ = V1_2::IBatteryInterface::Get();
        if(iBatteryInterface_ == nullptr) {
            QPOWER_LOGE("failed to get battery hdi interface");
            return false;
        }
    }
    QPOWER_LOGI("change path /sys/class/power_supply");
    iBatteryInterface_->ChangePath("/sys/class/power_supply");
    return true;
}
}  // namespace PowerMgr
}  // namespace OHOS