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
 * @file demo_xxxx_retcode.h
 *
 * @brief Enumerates the error codes for calling of demo xxxx.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef DEMO_XXXX_RETCODE_H
#define DEMO_XXXX_RETCODE_H

/**
 * @brief Enumerates the error codes for demo xxxx.
 *
 * @since 2.2
 * @version 1.0
 */
typedef enum {
    DEMO_XXXX_RETCODE_FAILURE = -1,                  /** Return code for an operation failure */
    DEMO_XXXX_RETCODE_SUCCESS = 0,                   /** Return code for an operation success */
    DEMO_XXXX_RETCODE_INIT_ERROR = 1001,             /** Return code for an initialization error */
    DEMO_XXXX_RETCODE_NULL_PARAM = 1002,             /** Return code for a null parameter error */
    DEMO_XXXX_RETCODE_SERIALIZATION_ERROR = 2001,    /** Return code for a serialization error */
    DEMO_XXXX_RETCODE_UNSERIALIZATION_ERROR = 2002,  /** Return code for a deserialization error */
    DEMO_XXXX_RETCODE_PLUGIN_EXECUTION_ERROR = 3001, /** Return code for a plugin execution error */
    DEMO_XXXX_RETCODE_PLUGIN_SESSION_ERROR = 3002,   /** Return code for a plugin session error */
} DemoXxxxRetCode;

#endif // DEMO_XXXX_RETCODE_H
/** @} */
