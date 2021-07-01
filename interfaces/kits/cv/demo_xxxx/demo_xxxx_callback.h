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

/**
 * @addtogroup demo_xxxx_sdk
 * @{
 *
 * @brief Defines the development tool functions, constants, and error codes for Demo xxxx (DEMO_XXXX).
 *
 * @since 2.2
 * @version 1.0
 */

/**
 * @file demo_xxxx_callback.h
 *
 * @brief Defines the callback for processing errors and prediction results of demo xxxx.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef DEMO_XXXX_CALLBACK_H
#define DEMO_XXXX_CALLBACK_H

#include "demo_xxxx_constants.h"
#include "demo_xxxx_retcode.h"

namespace OHOS {
namespace AI {
class DemoXxxxCallback {
public:
    /**
     * @brief Defines the destructor for the callback.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~DemoXxxxCallback() = default;

    /**
     * @brief Defines the callback for processing errors.
     *
     * @param errorCode Indicates the error code defined by {@link DemoXxxxRetCode} for the demo xxxx task.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual void OnError(const DemoXxxxRetCode errorCode) = 0;

    /**
     * @brief Defines the callback for prediction results.
     *
     * @param result Indicates the prediction result.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual void OnResult(const DemoXxxxOutput &result) = 0;
};
} // namespace AI
} // namespace OHOS

#endif // DEMO_XXXX_CALLBACK_H
/** @} */
