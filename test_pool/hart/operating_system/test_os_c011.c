/** @file
 * Copyright (c) 2016-2018,2021,2023, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#include "val/include/bsa_acs_val.h"
#include "val/include/bsa_acs_hart.h"

#define TEST_NUM   (ACS_PE_TEST_NUM_BASE  +  11)
#define TEST_RULE  "B_PE_11"
#define TEST_DESC  "Check num of Breakpoints and type     "

static
void
payload()
{
  uint64_t data = 0;
  int32_t  breakpointcount;
  uint32_t context_aware_breakpoints = 0;
  uint32_t hart_index = val_hart_get_index_mpid(val_hart_get_mpid());
  uint32_t primary_hart_idx = val_hart_get_primary_index();

  data = val_hart_reg_read(ID_AA64DFR0_EL1);

  /* bits 15:12 for Number of breakpoints - 1 */
  breakpointcount = VAL_EXTRACT_BITS(data, 12, 15) + 1;
  if (breakpointcount < 6) {
      if (hart_index == primary_hart_idx) {
          val_print(ACS_PRINT_ERR,
          "\n       Number of HART breakpoints reported: %d, expected >= 6", breakpointcount);
      }
      val_set_status(hart_index, RESULT_FAIL(TEST_NUM, 1));
      return;
  }

  /*bits [31:28] Number of breakpoints that are context-aware, minus 1*/
  context_aware_breakpoints = VAL_EXTRACT_BITS(data, 28, 31) + 1;
  if (context_aware_breakpoints > 1)
      val_set_status(hart_index, RESULT_PASS(TEST_NUM, 1));
  else {
      if (hart_index == primary_hart_idx) {
          val_print(ACS_PRINT_ERR,
          "\n       Number of HART context-aware breakpoints reported: %d, expected > 1",
          context_aware_breakpoints);
      }
      val_set_status(hart_index, RESULT_FAIL(TEST_NUM, 2));
  }
  return;

}

/**
  @brief   Check for the number of breakpoints available
**/
uint32_t
os_c011_entry(uint32_t num_hart)
{
  uint32_t status = ACS_STATUS_FAIL;

  status = val_initialize_test(TEST_NUM, TEST_DESC, num_hart);
  if (status != ACS_STATUS_SKIP)
      /* execute payload on present HART and then execute on other HART */
      val_run_test_payload(TEST_NUM, num_hart, payload, 0);

  /* get the result from all HART and check for failure */
  status = val_check_for_error(TEST_NUM, num_hart, TEST_RULE);

  val_report_status(0, BSA_ACS_END(TEST_NUM), NULL);

  return status;
}

