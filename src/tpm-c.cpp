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

/**
 * @author Cedric Hammes
 * @since  04/01/2024
 */

#include "tpm-cpp/tpm-c.h"
#include "tpm-cpp/tpm.hpp"
#include <stdexcept>
#include <vector>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

tpm_error_t tpm_context_init(tpm_object_t* context) {
    if (context->type != TPM_OBJECT_CONTEXT) {
        return TPM_ERROR_INVALID_OBJECT;
    }

    try {
        context->context.context = new tpm::TPMContext();
        return TPM_ERROR_SUCCESS;
    } catch (std::runtime_error& ignored) {
        return TPM_ERROR_OPERATION_FAILED;
    }
}

tpm_error_t tpm_context_message(const tpm_object_t* context, const uint8_t* msg, const uint32_t msg_length, const tpm_object* response) {
    if (context->type != TPM_OBJECT_CONTEXT && context->type != TPM_OBJECT_MESSAGE) {
        return TPM_ERROR_INVALID_OBJECT;
    }

    const auto* tpm_context = static_cast<tpm::TPMContext*>(context->context.context);
    const auto message = std::vector(msg, msg + msg_length);

    try {
        auto* response_vector = tpm_context->emit_message(message);
        auto response_object = response->message;
        response_object.length = response_vector->size();
        response_object.message = static_cast<void*>(response_vector);

    } catch (std::runtime_error& ignored) {
        return TPM_ERROR_OPERATION_FAILED;
    }

    return TPM_ERROR_SUCCESS;
}

TPMCPP_API void* tpm_message_get_data(const tpm_object_t* object) {
    if (object->type != TPM_OBJECT_MESSAGE) {
        return nullptr;
    }

    return object->message.message;
}

tpm_error_t tpm_context_close(const tpm_object_t* object) {
    if (object->type == TPM_OBJECT_MESSAGE) {
        delete static_cast<std::vector<uint8_t>*>(object->message.message);
    } else if (object->type == TPM_OBJECT_CONTEXT) {
        const auto context = static_cast<tpm::TPMContext*>(object->context.context);
        context->~TPMContext();
        delete context;
    }
    return TPM_ERROR_INVALID_OBJECT;
}

#ifdef __cplusplus
}
#endif
