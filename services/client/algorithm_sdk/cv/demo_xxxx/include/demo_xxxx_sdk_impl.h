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

#ifndef DEMO_XXXX_SDK_IMPL_H
#define DEMO_XXXX_SDK_IMPL_H

#include <vector>

#include "aie_algorithm_type.h"
#include "aie_info_define.h"
#include "constants.h"
#include "demo_xxxx_constants.h"
#include "demo_xxxx_sdk.h"
#include "single_instance.h"

namespace OHOS {
namespace AI {
class DemoXxxxSdkImpl {
DECLARE_SINGLE_INSTANCE(DemoXxxxSdkImpl);
public:
    int32_t Create();
    int32_t SetCallback(std::shared_ptr<DemoXxxxCallback> callback);
    int32_t SyncExecute(const DemoXxxxInput &pDemoXxxxInput);
    int32_t Destroy();
private:
    std::shared_ptr<DemoXxxxCallback> callback_ = nullptr;
    intptr_t demoXxxxHandle_ = INVALID_DEMO_XXXX_HANDLE;
    int32_t OnSyncExecute(const DemoXxxxInput &pDemoXxxxInput, DataInfo &outputInfo);
    ConfigInfo configInfo_ {
        .description = "DemoXxxx config description"
    };
    ClientInfo clientInfo_ {
        .clientVersion = CLIENT_VERSION_DEMO_XXXX,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = INVALID_SESSION_ID,
        .extendLen = EXT_MSG_LEN_DEFAULT,
        .extendMsg = nullptr
    };
    AlgorithmInfo algorithmInfo_ {
        .clientVersion = CLIENT_VERSION_DEMO_XXXX,
        .isAsync = false,
        .algorithmType = ALGORITHM_TYPE_DEMO_XXXX,
        .algorithmVersion = ALGOTYPE_VERSION_DEMO_XXXX,
        .isCloud = false,
        .operateId = STARTING_OPERATE_ID,
        .requestId = STARTING_REQ_ID,
        .extendLen = EXT_MSG_LEN_DEFAULT,
        .extendMsg = nullptr
    };
};
} // namespace AI
} // namespace OHOS
#endif // DEMO_XXXX_SDK_IMPL_H
