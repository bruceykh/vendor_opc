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

#ifndef DEVICE_SOC_QUALCOMN_SDM845_HARDWARE_QPOWER_SERVICE_ABILITY_H
#define DEVICE_SOC_QUALCOMN_SDM845_HARDWARE_QPOWER_SERVICE_ABILITY_H

#include "system_ability.h"

#include "system_ability_definition.h"
#include "refbase.h"
#include "v1_2/ibattery_interface.h"
#include "iservmgr_hdi.h"
#include "iservstat_listener_hdi.h"
#include "hdi_service_status_listener.h"

#define QPOWER_SERVICE_ABILITY_ID VENDOR_SYS_ABILITY_ID_BEGIN + 2

namespace OHOS {
namespace PowerMgr {
class QPowerServiceAbility final : public SystemAbility {
public:
    /**
     * @brief The constructor of service ability.
     *
     * @param systemAbilityId Indicates the system ability id.
     * @param runOnCreate Run the system ability on created.
     */
    QPowerServiceAbility(const int32_t systemAbilityId, bool runOnCreate);

    /**
     * @brief The deconstructor.
     */
    ~QPowerServiceAbility() final;

    DISALLOW_COPY_AND_MOVE(QPowerServiceAbility);
    DECLARE_SYSTEM_ABILITY(QPowerServiceAbility);

private:
    void OnStart() final;
    void OnStop() final;
    bool RegisterBatteryHdiCallback();
    bool RegisterHdiStatusListener();
    
    sptr<HDI::Battery::V1_2::IBatteryInterface> iBatteryInterface_ { nullptr };
    sptr<OHOS::HDI::ServiceManager::V1_0::IServiceManager> hdiServiceMgr_ { nullptr }; 
    sptr<HdiServiceStatusListener::IServStatListener> hdiServStatListener_ { nullptr };
};
}  // namespace PowerMgr
}  // namespace OHOS

#endif // DEVICE_SOC_QUALCOMN_SDM845_HARDWARE_QPOWER_SERVICE_ABILITY_H