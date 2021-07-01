/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LDEMO_XXXXENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "demo_xxxx_sdk_impl.h"

#include <algorithm>

#include "aie_guard.h"
#include "aie_retcode_inner.h"
#include "encdec_facade.h"
#include "i_aie_client.inl"
#include "demo_xxxx_retcode.h"
#include "plugin_helper.h"

using namespace OHOS::AI;
namespace {
    const uint32_t MAX_IPC_BUFFER_SIZE = 25600;
}

IMPLEMENT_SINGLE_INSTANCE(DemoXxxxSdkImpl);

DemoXxxxSdkImpl::DemoXxxxSdkImpl() = default;

DemoXxxxSdkImpl::~DemoXxxxSdkImpl() = default;

int32_t DemoXxxxSdkImpl::Create()
{
    HILOGI("[DemoXxxxSdkImpl]Start");
    if (demoXxxxHandle_ != INVALID_DEMO_XXXX_HANDLE) {
        HILOGE("[DemoXxxxSdkImpl]Do not create again");
        return DEMO_XXXX_RETCODE_INIT_ERROR;
    }
    int32_t retCode = AieClientInit(configInfo_, clientInfo_, algorithmInfo_, nullptr);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_INIT_ERROR))
                               : HILOGD("[DemoXxxxSdkImpl]No callback");
        HILOGE("[DemoXxxxSdkImpl]AieClientInit failed. Error code[%d]", retCode);
        return DEMO_XXXX_RETCODE_INIT_ERROR;
    }
    if (clientInfo_.clientId == INVALID_CLIENT_ID) {
        (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_INIT_ERROR))
                               : HILOGD("[DemoXxxxSdkImpl]No callback");
        HILOGE("[DemoXxxxSdkImpl]Fail to allocate client id");
        return DEMO_XXXX_RETCODE_INIT_ERROR;
    }
    DataInfo inputInfo = {.data = nullptr, .length = 0};
    DataInfo outputInfo = {.data = nullptr, .length = 0};
    retCode = AieClientPrepare(clientInfo_, algorithmInfo_, inputInfo, outputInfo, nullptr);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_INIT_ERROR))
                               : HILOGD("[DemoXxxxSdkImpl]No callback");
        HILOGE("[DemoXxxxSdkImpl]AieClientPrepare failed. Error code[%d]", retCode);
        return DEMO_XXXX_RETCODE_INIT_ERROR;
    }
    if (outputInfo.data == nullptr || outputInfo.length <= 0) {
        (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_INIT_ERROR))
                               : HILOGD("[DemoXxxxSdkImpl]No callback");
        HILOGE("[DemoXxxxSdkImpl]The data or length of output info is invalid");
        return DEMO_XXXX_RETCODE_INIT_ERROR;
    }
    MallocPointerGuard<unsigned char> pointerGuard(outputInfo.data);
    retCode = EncdecFacade::ProcessDecode(outputInfo, demoXxxxHandle_);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_UNSERIALIZATION_ERROR))
                               : HILOGD("[DemoXxxxSdkImpl]No callback");
        HILOGE("[DemoXxxxSdkImpl]Failed to UnSerializeHandle");
        return DEMO_XXXX_RETCODE_UNSERIALIZATION_ERROR;
    }
    return DEMO_XXXX_RETCODE_SUCCESS;
}

int32_t DemoXxxxSdkImpl::OnSyncExecute(const DemoXxxxInput &inputData, DataInfo &outputInfo)
{
    if (inputData.data == nullptr || inputData.size == 0) {
        HILOGE("[DemoXxxxSdkImpl]Empty input");
        return DEMO_XXXX_RETCODE_NULL_PARAM;
    }
    DataInfo inputInfo = {
        .data = nullptr,
        .length = 0
    };
    DemoXxxxInput tmpImage = {
        .data = nullptr,
        .size = MAX_IPC_BUFFER_SIZE
    };
    uint32_t offset = 0;
    int32_t retCode = RETCODE_SUCCESS;
    while (offset < inputData.size) {
        tmpImage.data = &inputData.data[offset];
        tmpImage.size = std::min(inputData.size - offset, MAX_IPC_BUFFER_SIZE);
        inputInfo.data = nullptr;
        inputInfo.length = 0;
        outputInfo.data = nullptr;
        outputInfo.length = 0;
        retCode = EncdecFacade::ProcessEncode(inputInfo, demoXxxxHandle_, offset, tmpImage);
        if (retCode != RETCODE_SUCCESS) {
            (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_SERIALIZATION_ERROR))
                                   : HILOGD("[DemoXxxxSdkImpl]No callback");
            HILOGE("[DemoXxxxSdkImpl]Failed to UnSerializeHandle");
            return DEMO_XXXX_RETCODE_SERIALIZATION_ERROR;
        }
        retCode = AieClientSyncProcess(clientInfo_, algorithmInfo_, inputInfo, outputInfo);
        if (retCode != RETCODE_SUCCESS) {
            (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_FAILURE))
                                   : HILOGD("[DemoXxxxSdkImpl]No callback");
            HILOGE("[DemoXxxxSdkImpl]SyncExecute AieClientSyncProcess failed");
            return DEMO_XXXX_RETCODE_FAILURE;
        }
        offset += tmpImage.size;
    }
    return DEMO_XXXX_RETCODE_SUCCESS;
}

int32_t DemoXxxxSdkImpl::SyncExecute(const DemoXxxxInput &inputData)
{
    HILOGI("[DemoXxxxSdkImpl]Start");
    DataInfo outputInfo = {.data = nullptr, .length = 0};
    int32_t retCode = OnSyncExecute(inputData, outputInfo);
    if (outputInfo.data == nullptr || outputInfo.length <= 0 || retCode != DEMO_XXXX_RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_FAILURE))
                               : HILOGD("[DemoXxxxSdkImpl]No callback");
        HILOGE("[DemoXxxxSdkImpl]SyncExecute failed");
        return retCode;
    }
    DemoXxxxOutput demoXxxxResult = {.data = nullptr, .size = 0};
    intptr_t receivedHandle = INVALID_DEMO_XXXX_HANDLE;
    MallocPointerGuard<unsigned char> pointerGuard(outputInfo.data);
    retCode = EncdecFacade::ProcessDecode(outputInfo, receivedHandle, demoXxxxResult);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_UNSERIALIZATION_ERROR))
                               : HILOGD("[DemoXxxxSdkImpl]No callback");
        HILOGE("[DemoXxxxSdkImpl]Failed to UnSerializeHandle");
        return DEMO_XXXX_RETCODE_UNSERIALIZATION_ERROR;
    }
    if (demoXxxxHandle_ != receivedHandle) {
        (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_FAILURE))
                               : HILOGD("[DemoXxxxSdkImpl]No callback");
        HILOGE("[DemoXxxxSdkImpl]The handle[%lld] of output data is not equal to the current handle[%lld]",
            (long long)receivedHandle, (long long)demoXxxxHandle_);
        return DEMO_XXXX_RETCODE_FAILURE;
    }
    (callback_ != nullptr) ? (callback_->OnResult(demoXxxxResult))
                           : HILOGD("[DemoXxxxSdkImpl]No callback");
    return DEMO_XXXX_RETCODE_SUCCESS;
}

int32_t DemoXxxxSdkImpl::SetCallback(std::shared_ptr<DemoXxxxCallback> callback)
{
    if (callback == nullptr) {
        return DEMO_XXXX_RETCODE_FAILURE;
    }
    callback_ = callback;
    return DEMO_XXXX_RETCODE_SUCCESS;
}

int32_t DemoXxxxSdkImpl::Destroy()
{
    HILOGI("[DemoXxxxSdkImpl]Destroy");
    if (demoXxxxHandle_ == INVALID_DEMO_XXXX_HANDLE) {
        return DEMO_XXXX_RETCODE_SUCCESS;
    }
    DataInfo inputInfo = {.data = nullptr, .length = 0};
    int32_t retCode = EncdecFacade::ProcessEncode(inputInfo, demoXxxxHandle_);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_SERIALIZATION_ERROR))
                               : HILOGD("[DemoXxxxSdkImpl]No callback");
        HILOGE("[DemoXxxxSdkImpl]Failed to SerializeHandle");
        return DEMO_XXXX_RETCODE_SERIALIZATION_ERROR;
    }
    retCode = AieClientRelease(clientInfo_, algorithmInfo_, inputInfo);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_FAILURE))
                               : HILOGD("[DemoXxxxSdkImpl]No callback");
        HILOGE("[DemoXxxxSdkImpl]AieClientRelease failed. Error code[%d]", retCode);
        return DEMO_XXXX_RETCODE_FAILURE;
    }
    retCode = AieClientDestroy(clientInfo_);
    demoXxxxHandle_ = INVALID_DEMO_XXXX_HANDLE;
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(DEMO_XXXX_RETCODE_FAILURE))
                               : HILOGD("[DemoXxxxSdkImpl]No callback");
        HILOGE("[DemoXxxxSdkImpl]AieClientDestroy failed. Error code[%d]", retCode);
        return DEMO_XXXX_RETCODE_FAILURE;
    }
    return DEMO_XXXX_RETCODE_SUCCESS;
}
