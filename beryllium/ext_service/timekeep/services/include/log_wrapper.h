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

#ifndef DEVICE_SOC_QUALCOMN_SDM845_HARDWARE_TIMEKEEP_LOG_HELPER_H
#define DEVICE_SOC_QUALCOMN_SDM845_HARDWARE_TIMEKEEP_LOG_HELPER_H

#include <stdint.h>         // for uint8_t
#include <string>           // for basic_string

#include "hilog/log.h"

#undef LOG_TAG
#undef LOG_DOMAIN
#define LOG_TAG "TIMEKEEP_SA"
#define LOG_DOMAIN 0xD001400

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)

#ifndef TIME_LOGD
#define TIME_LOGD(format, ...)                                                                                     \
    do {                                                                                                              \
        HILOG_DEBUG(LOG_CORE, "[%{public}s@%{public}s:%{public}d] " format "\n",                                      \
            __FUNCTION__, __FILENAME__, __LINE__,                                                                     \
            ##__VA_ARGS__);                                                                                           \
    } while (0)
#endif

#ifndef TIME_LOGI
#define TIME_LOGI(format, ...)                                                                                     \
    do {                                                                                                              \
        HILOG_INFO(LOG_CORE, "[%{public}s@%{public}s:%{public}d] " format "\n", __FUNCTION__, __FILENAME__, __LINE__, \
            ##__VA_ARGS__);                                                                                           \
    } while (0)
#endif

#ifndef TIME_LOGW
#define TIME_LOGW(format, ...)                                                                                     \
    do {                                                                                                              \
        HILOG_WARN(LOG_CORE, "[%{public}s@%{public}s:%{public}d] " format "\n", __FUNCTION__, __FILENAME__, __LINE__, \
            ##__VA_ARGS__);                                                                                           \
    } while (0)
#endif

#ifndef TIME_LOGE
#define TIME_LOGE(format, ...)                                 \
    do {                                                          \
        HILOG_ERROR(LOG_CORE,                                     \
            "\033[0;32;31m"                                       \
            "[%{public}s@%{public}s:%{public}d] " format "\033[m" \
            "\n",                                                 \
            __FUNCTION__, __FILENAME__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#endif

#endif  // DEVICE_SOC_QUALCOMN_SDM845_HARDWARE_TIMEKEEP_LOG_HELPER_H