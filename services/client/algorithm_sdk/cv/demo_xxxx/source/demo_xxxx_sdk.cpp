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

#include "demo_xxxx_sdk.h"

#include "demo_xxxx_sdk_impl.h"

using namespace OHOS::AI;

DemoXxxxSdk::~DemoXxxxSdk()
{
    DemoXxxxSdkImpl::GetInstance().Destroy();
}

int32_t DemoXxxxSdk::Create()
{
    return DemoXxxxSdkImpl::GetInstance().Create();
}

int32_t DemoXxxxSdk::SetCallback(std::shared_ptr<DemoXxxxCallback> callback)
{
    return DemoXxxxSdkImpl::GetInstance().SetCallback(callback);
}

int32_t DemoXxxxSdk::SyncExecute(const DemoXxxxInput &inputData)
{
    return DemoXxxxSdkImpl::GetInstance().SyncExecute(inputData);
}

int32_t DemoXxxxSdk::Destroy()
{
    return DemoXxxxSdkImpl::GetInstance().Destroy();
}
