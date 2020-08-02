// ----------------------------------------------------------------------------
// Copyright 2018-2020 ARM Ltd.
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

#ifndef __FOTA_COMPONENT_INTERNAL_H_
#define __FOTA_COMPONENT_INTERNAL_H_

#include "fota/fota_base.h"
#include "fota/fota_component.h"

#ifdef __cplusplus
extern "C" {
#endif

// Component access APIs
void fota_component_clean(void);
unsigned int fota_component_num_components(void);
void fota_component_get_desc(unsigned int comp_id, const fota_component_desc_t * *comp_desc);
void fota_component_get_curr_version(unsigned int comp_id, fota_component_version_t *version);
void fota_component_set_curr_version(unsigned int comp_id, fota_component_version_t version);

int fota_component_name_to_id(const char *name, unsigned int *comp_id);

// Semantic version translation
int fota_component_version_int_to_semver(fota_component_version_t version, char *sem_ver);
int fota_component_version_semver_to_int(const char *sem_ver, fota_component_version_t *version);

#ifdef __cplusplus
}
#endif

#endif // __FOTA_COMPONENT_INTERNAL_H_
