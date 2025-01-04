// Copyright 2025 Cedric Hammes
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
/**
 * @author Cedric Hammes
 * @since  04/01/2024
 */

#include <stdint.h>

#ifdef COMPILER_MSVC
#define TPMCPP_API __declspec(dllexport)
#else
#define TPMCPP_API __attribute__((visibility("default")))
#endif

#define TPM_OBJECT_CONTEXT 1
#define TPM_OBJECT_MESSAGE 2

#define TPM_ERROR_SUCCESS 0
#define TPM_ERROR_INVALID_OBJECT (-1)
#define TPM_ERROR_OPERATION_FAILED (-2)

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t tpm_error_t;

typedef struct tpm_message {
    int32_t type;
    uint32_t length;
    void* message;
} tpm_message_t;

typedef struct tpm_context {
    int32_t type;
    void* context;
} tpm_context_t;

typedef struct tpm_object {
    int32_t type;
    tpm_message_t message;
    tpm_context_t context;
} tpm_object_t;

TPMCPP_API tpm_error_t tpm_context_init(tpm_object_t* context);
TPMCPP_API tpm_error_t tpm_context_message(const tpm_object_t* context, const uint8_t* msg, uint32_t msg_length, const tpm_object_t* response);
TPMCPP_API void* tpm_message_get_data(const tpm_object_t* object);
TPMCPP_API tpm_error_t tpm_context_close(const tpm_object_t* object);

#ifdef __cplusplus
}
#endif
