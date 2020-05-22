// ----------------------------------------------------------------------------
// Copyright 2018-2019 ARM Ltd.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------

#include "fota/fota_base.h"

#ifdef MBED_CLOUD_CLIENT_FOTA_ENABLE

#include "fota/fota_block_device.h"

// External BD should supply all these APIs

#if (MBED_CLOUD_CLIENT_FOTA_BLOCK_DEVICE_TYPE != FOTA_EXTERNAL_BD)

#ifndef __MBED__
#error This type of block device is only supported in mbed-os
#endif

static bool initialized = false;

#include "BlockDevice.h"

#if (MBED_CLOUD_CLIENT_FOTA_BLOCK_DEVICE_TYPE == FOTA_INTERNAL_FLASH_BD)
#if COMPONENT_FLASHIAP
#include "FlashIAPBlockDevice.h"
#else
#error FlashIAP component should be defined in case of an internal flash block device configuration
#endif
#endif // (MBED_CLOUD_CLIENT_FOTA_BLOCK_DEVICE_TYPE == FOTA_INTERNAL_FLASH_BD)

#include <string.h>

static mbed::BlockDevice *bd = 0;

#if (MBED_CLOUD_CLIENT_FOTA_BLOCK_DEVICE_TYPE == FOTA_CUSTOM_BD)
// In custom BD case, the user code should supply this function, returning the desired block device
mbed::BlockDevice *fota_bd_get_custom_bd();
#elif (MBED_CLOUD_CLIENT_FOTA_BLOCK_DEVICE_TYPE == FOTA_INTERNAL_FLASH_BD)
mbed::BlockDevice *fota_bd_get_custom_bd()
{
    if (!bd) {
        bd = new FlashIAPBlockDevice(MBED_ROM_START, MBED_ROM_SIZE);
    }
    return bd;
}
#endif

// This ifdef is here (always true) to prevent astyle from indenting enclosed functions
#ifdef __cplusplus
extern "C" {
#endif

int fota_bd_create(uint32_t size, uint32_t read_size, uint32_t prog_size,
                   uint32_t erase_size)
{
    return 0;
}

uint32_t fota_bd_size()
{
    if (!initialized) {
        int ret = fota_bd_init();
        if (ret) {
            return 0;
        }
    }

    return bd->size();
}

int fota_bd_init(void)
{
    if (initialized) {
        return 0;
    }

    if (!bd) {
        bd = fota_bd_get_custom_bd();
    }
    FOTA_ASSERT(bd);

    int ret = bd->init();
    if (!ret) {
        initialized = true;
    }
    return ret;
}

int fota_bd_deinit(void)
{
    if (!initialized) {
        return 0;
    }

    int ret = bd->deinit();
#if (MBED_CLOUD_CLIENT_FOTA_BLOCK_DEVICE_TYPE == FOTA_INTERNAL_FLASH_BD)
    delete bd;
#endif
    bd = 0;
    initialized = false;
    return ret;
}

int fota_bd_read(void *buffer, uint32_t addr, uint32_t size)
{
    if (!initialized) {
        int ret = fota_bd_init();
        if (ret) {
            return ret;
        }
    }

    return bd->read(buffer, addr, size);
}

int fota_bd_program(const void *buffer, uint32_t addr, uint32_t size)
{
    if (!initialized) {
        int ret = fota_bd_init();
        if (ret) {
            return ret;
        }
    }

    return bd->program(buffer, addr, size);
}

int fota_bd_erase(uint32_t addr, uint32_t size)
{
    if (!initialized) {
        int ret = fota_bd_init();
        if (ret) {
            return ret;
        }
    }

    return bd->erase(addr, size);
}

uint32_t fota_bd_get_read_size(void)
{
    if (!initialized) {
        int ret = fota_bd_init();
        if (ret) {
            return 0;
        }
    }

    return bd->get_read_size();
}

uint32_t fota_bd_get_program_size(void)
{
    if (!initialized) {
        int ret = fota_bd_init();
        if (ret) {
            return 0;
        }
    }

    return bd->get_program_size();
}

uint32_t fota_bd_get_erase_size(uint32_t addr)
{
    if (!initialized) {
        int ret = fota_bd_init();
        if (ret) {
            return 0;
        }
    }

    return bd->get_erase_size(addr);
}

uint8_t fota_bd_get_erase_value(void)
{
    if (!initialized) {
        int ret = fota_bd_init();
        if (ret) {
            return 0;
        }
    }

    return bd->get_erase_value();
}

#ifdef __cplusplus
}
#endif

#endif // (MBED_CLOUD_CLIENT_FOTA_BLOCK_DEVICE_TYPE != FOTA_EXTERNAL_BD)

static bool is_internal_flash_bd()
{
#if (MBED_CLOUD_CLIENT_FOTA_BLOCK_DEVICE_TYPE == FOTA_INTERNAL_FLASH_BD)
    return true;
#elif (MBED_CLOUD_CLIENT_FOTA_BLOCK_DEVICE_TYPE == FOTA_CUSTOM_BD)
    FOTA_ASSERT(bd);
    const char *bd_type = bd->get_type();
    if (strcmp("FLASHIAP", bd_type) == 0) {
        return true;
    }
    return false;
#else
    return false;
#endif
}

extern "C" uint32_t fota_bd_physical_addr_to_logical_addr(uint32_t phys_addr)
{
#ifdef __MBED__
    if (is_internal_flash_bd()) {
        return phys_addr - MBED_ROM_START;
    }
#endif
    return phys_addr;
}

#endif  // MBED_CLOUD_CLIENT_FOTA_ENABLE