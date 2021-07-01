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

#include "demo_xxxx_plugin.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "aie_log.h"
#include "aie_retcode_inner.h"
#include "encdec_facade.h"
#include "demo_xxxx_constants.h"
#include "plugin_helper.h"
#include "securec.h"

#ifdef USE_NNIE
#include "nnie_adapter.h"
#endif

using namespace std;

namespace OHOS {
namespace AI {
namespace {
    const std::string PLUGIN_MODEL_PATH = "/storage/data/demo_xxxx.wk";
    const std::string DEFAULT_INFER_MODE = "SYNC";
    const std::string ALGORITHM_NAME_DEMO_XXXX = "DEMO_XXXX";
    const int32_t OPTION_GET_INPUT_SIZE = 1001;
    const int32_t OPTION_GET_OUTPUT_SIZE = 1002;
    const int32_t OPTION_SET_OUTPUT_SIZE = 2002;
    const uint16_t MODEL_INPUT_NODE_ID = 0;
    const uint16_t MODEL_OUTPUT_NODE_ID = 0;
    const uint16_t DEFAULT_OUTPUT_SIZE = 5;
    const intptr_t EMPTY_UINTPTR = 0;
    using Item = std::pair<int32_t, int32_t>;
    using Items = std::vector<Item>;
    int32_t GetTopK(const int32_t *data, size_t size, size_t topK, Items &result)
    {
        if (data == nullptr) {
            HILOGE("[DemoXxxxPlugin]Fail with null data pointer");
            return RETCODE_FAILURE;
        }
        if (topK > size) {
            topK = size;
        }
        size_t index = 0;
        while (index < topK) {
            result.emplace_back(index, data[index]);
            index++;
        }
        const auto heapComparer = [](const Item &x, const Item &y) {
            return (x.second > y.second);
        };
        std::make_heap(result.begin(), result.end(), heapComparer);
        while (index < size) {
            if (result.front().second < data[index]) {
                std::pop_heap(result.begin(), result.end(), heapComparer);
                result.pop_back();
                result.emplace_back(index, data[index]);
                std::push_heap(result.begin(), result.end(), heapComparer);
            }
            index++;
        }
        std::sort_heap(result.begin(), result.end(), heapComparer);
        return RETCODE_SUCCESS;
    }
}

DemoXxxxPlugin::DemoXxxxPlugin() : adapter_(nullptr)
{
    HILOGD("[DemoXxxxPlugin]Ctor");
    handles_.clear();
}

DemoXxxxPlugin::~DemoXxxxPlugin()
{
    HILOGD("[DemoXxxxPlugin]Dtor");
    ReleaseAllHandles();
}

int32_t DemoXxxxPlugin::Prepare(long long transactionId, const DataInfo &inputInfo, DataInfo &outputInfo)
{
    HILOGI("[DemoXxxxPlugin]Start to prepare, transactionId = %lld", transactionId);
    if (adapter_ == nullptr) {
#ifdef USE_NNIE
        adapter_ = std::make_shared<NNIEAdapter>();
#endif
        if (adapter_ == nullptr) {
            HILOGE("[DemoXxxxPlugin]Fail to create engine adapter");
            return RETCODE_FAILURE;
        }
    }
    intptr_t handle = EMPTY_UINTPTR;
    if (adapter_->Init(PLUGIN_MODEL_PATH.c_str(), handle) != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]EngineAdapterInit failed");
        return RETCODE_FAILURE;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    const auto iter = handles_.find(handle);
    if (iter != handles_.end()) {
        HILOGE("[DemoXxxxPlugin]Handle=%lld has already existed", static_cast<long long>(handle));
        return RETCODE_SUCCESS;
    }
    DemoXxxxPluginConfig config;
    if (BuildConfig(handle, config) != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]BuildConfig failed");
        return RETCODE_FAILURE;
    }
    handles_.emplace(handle, config);
    return EncdecFacade::ProcessEncode(outputInfo, handle);
}

int32_t DemoXxxxPlugin::Release(bool isFullUnload, long long transactionId, const DataInfo &inputInfo)
{
    if (adapter_ == nullptr) {
        HILOGE("[DemoXxxxPlugin]The engine adapter has not been created");
        return RETCODE_FAILURE;
    }
    HILOGI("[DemoXxxxPlugin]Begin to release, transactionId = %lld", transactionId);
    intptr_t handle = EMPTY_UINTPTR;
    int32_t retCode = EncdecFacade::ProcessDecode(inputInfo, handle);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]UnSerializeHandle Failed");
        return RETCODE_FAILURE;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    const auto iter = handles_.find(handle);
    if (iter == handles_.end()) {
        HILOGE("[DemoXxxxPlugin]Fail to find handle(%lld)", static_cast<long long>(handle));
        return RETCODE_NULL_PARAM;
    }
    retCode = adapter_->ReleaseHandle(handle);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]ReleaseHandle failed");
        return RETCODE_FAILURE;
    }
    handles_.erase(iter);
    if (isFullUnload) {
        retCode = adapter_->Deinit();
        if (retCode != RETCODE_SUCCESS) {
            HILOGE("[DemoXxxxPlugin]Engine adapter deinit failed");
            return RETCODE_FAILURE;
        }
    }
    return RETCODE_SUCCESS;
}

void DemoXxxxPlugin::ReleaseAllHandles()
{
    for (auto iter = handles_.begin(); iter != handles_.end(); ++iter) {
        (void)adapter_->ReleaseHandle(iter->first);
    }
    adapter_->Deinit();
    handles_.clear();
}

const long long DemoXxxxPlugin::GetVersion() const
{
    return ALGOTYPE_VERSION_DEMO_XXXX;
}

const char *DemoXxxxPlugin::GetName() const
{
    return ALGORITHM_NAME_DEMO_XXXX.c_str();
}

const char *DemoXxxxPlugin::GetInferMode() const
{
    return DEFAULT_INFER_MODE.c_str();
}

int32_t DemoXxxxPlugin::SetOption(int32_t optionType, const DataInfo &inputInfo)
{
    if (inputInfo.data == nullptr || inputInfo.length <= 0) {
        HILOGE("[DemoXxxxPlugin]Fail to set option with empty input info");
        return RETCODE_NULL_PARAM;
    }
    intptr_t handle = EMPTY_UINTPTR;
    uint32_t tmpUInt32Val = 0;
    int32_t retCode = RETCODE_SUCCESS;
    DemoXxxxPluginConfig newConfig;
    newConfig.outputSize = 0;
    switch (optionType) {
        case OPTION_SET_OUTPUT_SIZE:
            retCode = EncdecFacade::ProcessDecode(inputInfo, handle, tmpUInt32Val);
            if (retCode != RETCODE_SUCCESS) {
                HILOGE("[DemoXxxxPlugin]Fail to unserialize output size");
                return retCode;
            }
            newConfig.outputSize = tmpUInt32Val;
            break;
        default:
            HILOGE("[DemoXxxxPlugin]OptionType[%d] is not supported", optionType);
            return RETCODE_FAILURE;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    const auto iter = handles_.find(handle);
    if (iter == handles_.end()) {
        HILOGE("[DemoXxxxPlugin]No matched handle[%lld]", static_cast<long long>(handle));
        return RETCODE_FAILURE;
    }
    switch (optionType) {
        case OPTION_SET_OUTPUT_SIZE:
            if (newConfig.outputSize > 0 && newConfig.outputSize <= iter->second.maxOutputSize) {
                iter->second.outputSize = newConfig.outputSize;
            }
            break;
        default:
            break;
    }
    return RETCODE_SUCCESS;
}

int32_t DemoXxxxPlugin::GetOption(int32_t optionType, const DataInfo &inputInfo, DataInfo &outputInfo)
{
    if (inputInfo.data == nullptr || inputInfo.length <= 0) {
        HILOGE("[DemoXxxxPlugin]Fail to set option with empty input info");
        return RETCODE_NULL_PARAM;
    }
    intptr_t handle = EMPTY_UINTPTR;
    int32_t retCode = EncdecFacade::ProcessDecode(inputInfo, handle);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]Fail to get handle from input info");
        return retCode;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    const auto iter = handles_.find(handle);
    if (iter == handles_.end()) {
        HILOGE("[DemoXxxxPlugin]No matched handle [%lld]", static_cast<long long>(handle));
        return RETCODE_FAILURE;
    }
    outputInfo.length = 0;
    switch (optionType) {
        case OPTION_GET_INPUT_SIZE:
            return EncdecFacade::ProcessEncode(outputInfo, handle, iter->second.inputSize);
        case OPTION_GET_OUTPUT_SIZE:
            return EncdecFacade::ProcessEncode(outputInfo, handle, iter->second.outputSize);
        default:
            HILOGE("[DemoXxxxPlugin]GetOption optionType[%d] undefined", optionType);
            return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int32_t DemoXxxxPlugin::BuildConfig(intptr_t handle, DemoXxxxPluginConfig &config)
{
    int32_t retCode = adapter_->GetInputAddr(handle, MODEL_INPUT_NODE_ID, config.inputAddr, config.inputSize);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]GetInputAddr failed with error code[%d]", retCode);
        return RETCODE_FAILURE;
    }
    retCode = adapter_->GetOutputAddr(handle, MODEL_OUTPUT_NODE_ID, config.outputAddr, config.maxOutputSize);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]GetOutputAddr failed with error code[%d]", retCode);
        return RETCODE_FAILURE;
    }
    config.outputSize = DEFAULT_OUTPUT_SIZE;
    return RETCODE_SUCCESS;
}

int32_t DemoXxxxPlugin::SyncProcess(IRequest *request, IResponse *&response)
{
    if (adapter_ == nullptr) {
        HILOGE("[DemoXxxxPlugin]The engine adapter has not been created");
        return RETCODE_FAILURE;
    }
    if (request == nullptr) {
        HILOGE("[DemoXxxxPlugin]Fail to synchronously process with nullptr request");
        return RETCODE_NULL_PARAM;
    }
    DataInfo inputInfo = request->GetMsg();
    if (inputInfo.data == nullptr || inputInfo.length <= 0) {
        HILOGE("[DemoXxxxPlugin]Fail to synchronously process with empty input info");
        return RETCODE_NULL_PARAM;
    }
    intptr_t handle = EMPTY_UINTPTR;
    uint32_t slicedIndex = 0;
    Array<uint8_t> slicedImage = {0};
    int32_t retCode = EncdecFacade::ProcessDecode(inputInfo, handle, slicedIndex, slicedImage);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]Fail to unserialize input data");
        return retCode;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    const auto iter = handles_.find(handle);
    if (iter == handles_.end()) {
        HILOGE("[DemoXxxxPlugin]No matched handle [%lld]", static_cast<long long>(handle));
        return RETCODE_FAILURE;
    }
    if (slicedIndex + slicedImage.size > iter->second.inputSize) {
        HILOGE("[DemoXxxxPlugin]Illegal slicedIndex");
        return RETCODE_FAILURE;
    }
    auto imageAddr = reinterpret_cast<uint8_t *>(iter->second.inputAddr);
    errno_t retCopy = memcpy_s(&imageAddr[slicedIndex], iter->second.inputSize - slicedIndex,
        slicedImage.data, slicedImage.size);
    if (retCopy != EOK) {
        HILOGE("[DemoXxxxPlugin]Fail to copy sliced data to model input space");
        return RETCODE_FAILURE;
    }
    if (slicedIndex + slicedImage.size == iter->second.inputSize) {
        retCode = DoProcess(handle, iter->second, request, response);
        if (retCode != RETCODE_SUCCESS) {
            HILOGE("[DemoXxxxPlugin]Fail to do process");
            return retCode;
        }
    }
    return RETCODE_SUCCESS;
}

int32_t DemoXxxxPlugin::DoProcess(intptr_t handle, const DemoXxxxPluginConfig &config, IRequest *request, IResponse *&response)
{
    DataInfo outputInfo = {0};
    int32_t retCode = MakeInference(handle, config, outputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]Fail to make inference");
        return retCode;
    }
    response = IResponse::Create(request);
    response->SetResult(outputInfo);
    return RETCODE_SUCCESS;
}

int32_t DemoXxxxPlugin::AsyncProcess(IRequest *request, IPluginCallback *callback)
{
    return RETCODE_SUCCESS;
}

int32_t DemoXxxxPlugin::MakeInference(intptr_t handle, const DemoXxxxPluginConfig &config, DataInfo &outputInfo)
{
    HILOGI("[DemoXxxxPlugin]Start with handle = %lld", static_cast<long long>(handle));
    int32_t retCode = adapter_->Invoke(handle);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]MakeInference failed");
        return RETCODE_FAILURE;
    }
    // Return top K
    int32_t *outputData = reinterpret_cast<int32_t *>(config.outputAddr);
    vector<pair<int32_t, int32_t>> result;
    result.clear();
    retCode = GetTopK(outputData, config.maxOutputSize, config.outputSize, result);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]Fail to get TopK");
        return retCode;
    }
    retCode = EncdecFacade::ProcessEncode(outputInfo, handle, result);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[DemoXxxxPlugin]Fail to serialize output data");
    }
    return retCode;
}

PLUGIN_INTERFACE_IMPL(DemoXxxxPlugin);
} // namespace AI
} // namespace OHOS
