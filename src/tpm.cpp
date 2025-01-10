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

#include "tpm-cpp/tpm.hpp"
#include <stdexcept>
#include <fmt/format.h>

#ifdef PLATFORM_LINUX
#include <fcntl.h>
#include <unistd.h>

#define INVALID_HANDLE_VALUE (-1)
#endif

namespace tpm {
    TPMContext::TPMContext() {
#ifdef PLATFORM_LINUX
        _handle = open("/dev/tpmrm0", O_RDWR);
        if (_handle < 0) {
            throw std::runtime_error(fmt::format("Unable to open TPM device: {}", strerror(errno)));
        }
#else
        TBS_CONTEXT_PARAMS2 params {};
        params.version = TBS_CONTEXT_VERSION_TWO;
        params.includeTpm20 = 1;
        if (const auto result = Tbsi_Context_Create(reinterpret_cast<PTBS_CONTEXT_PARAMS>(&params), &_handle)) {
            throw std::runtime_error(fmt::format("Unable to open TPM device: {}", result));
        }
#endif
    }

    TPMContext::TPMContext(TPMContext &&other) noexcept : _handle(other._handle) {
        other._handle = INVALID_HANDLE_VALUE;
    }

    TPMContext::~TPMContext() noexcept {
#ifdef PLATFORM_LINUX
        if (_handle >= 0) {
            close(_handle);
            _handle = INVALID_HANDLE_VALUE;
        }
#else
        if (_handle != INVALID_HANDLE_VALUE) {
            Tbsip_Context_Close(_handle);
            _handle = INVALID_HANDLE_VALUE;
        }
#endif
    }

    auto TPMContext::emit_message(const std::vector<std::uint8_t> &message) const -> std::vector<std::uint8_t>* {
#ifdef PLATFORM_LINUX
        if (const std::size_t written = write(_handle, message.data(), message.size()); written != message.size()) {
            throw std::runtime_error(fmt::format("Unable to write to TPM device: {}", strerror(errno)));
        }

        // Reading the header of the TPM 2.0 message, then extracting the length out of it, and then we can expand the
        // buffer and read the remaining bytes of the message. (6 = length of response header without response code)
        auto response = new std::vector<std::uint8_t>(6);
        if (const auto read = ::read(_handle, response->data(), 6); read != 6) {
            throw std::runtime_error(fmt::format("Unable to read header of response: {}", strerror(errno)));
        }

        const std::size_t resp_length = (*response)[2] << 24 | (*response)[3] << 16 | (*response)[4] << 8 | (*response)[5];
        response->resize(resp_length);
        if (const auto read = ::read(_handle, response->data() + 6, resp_length - 6); read != resp_length - 6) {
            throw std::runtime_error(fmt::format("Unable to read remaining message: {}", strerror(errno)));
        }
#else
        std::uint32_t length = 64;
        auto response = new std::vector<std::uint8_t>(length);

        read:
        const auto result = Tbsip_Submit_Command(_handle,TBS_COMMAND_LOCALITY_ZERO, TBS_COMMAND_PRIORITY_NORMAL,
            message.data(), message.size(), response->data(), &length);
        if (result != TBS_SUCCESS) {
            if (result != TBS_E_INSUFFICIENT_BUFFER) {
                response->resize(length);
                goto read;
            }
            throw std::runtime_error(fmt::format("Unable to emit TPM message: {}", result));
        }
        response->resize(length);
#endif

        return response;
    }

    auto TPMContext::operator=(TPMContext &&other) noexcept -> TPMContext& {
        _handle = other._handle;
        other._handle = INVALID_HANDLE_VALUE;
        return *this;
    }
}
