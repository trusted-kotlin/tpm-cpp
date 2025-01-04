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

#include <vector>
#include <cstdint>

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <tbs.h>
#endif

namespace tpm {
#ifdef PLATFORM_WINDOWS
        typedef TBS_HCONTEXT TPMContextHandle;
#elif PLATFORM_LINUX
    typedef int TPMContextHandle;
#endif

    /**
     * @author Cedric Hammes
     * @since  04/01/2024
     */
    class TPMContext {
        TPMContextHandle _handle;

    public:
        explicit TPMContext();
        TPMContext(TPMContext &&other) noexcept;
        TPMContext(TPMContext &other) noexcept = delete;
        ~TPMContext() noexcept;

        [[nodiscard]] auto emit_message(const std::vector<std::uint8_t>& message) const -> std::vector<std::uint8_t>*;

        auto operator=(TPMContext &&other) noexcept -> TPMContext&;
        auto operator=(TPMContext &other) noexcept -> TPMContext& = delete;
    };
}
