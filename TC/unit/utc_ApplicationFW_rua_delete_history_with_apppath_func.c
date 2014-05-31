/*
 *  RUA
 *
 * Copyright (c) 2000 - 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Jayoun Lee <airjany@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <tet_api.h>
#include <rua.h>
#include <unistd.h>

static void startup(void);
static void cleanup(void);

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

static void utc_rua_delete_history_with_apppath_func_01(void);
static void utc_rua_delete_history_with_apppath_func_02(void);

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

struct tet_testlist tet_testlist[] = {
	{utc_rua_delete_history_with_apppath_func_01, POSITIVE_TC_IDX},
	{utc_rua_delete_history_with_apppath_func_02, NEGATIVE_TC_IDX},
	{NULL, 0}
};

static void startup(void)
{
	tet_infoline("Testing API rua_delete_history_with_apppath start");
	return;
}

static void cleanup(void)
{
	tet_infoline("Testing API rua_delete_history_with_apppath ends ");
	rua_clear_history();
	rua_fini();
	return;
}

/**
 * @brief Positive test case of rua_delete_history_with_apppath()
 */
static void utc_rua_delete_history_with_apppath_func_01(void)
{
	int ret = 0;
	struct rua_rec rec;
	char *pkgname = "com.samsung.testapp";
	char *apppath = "/opt/apps/com.samsung.testapp/bin/testapp";

	memset(&rec, 0, sizeof(rec));
	rec.pkg_name = pkgname;
	rec.app_path = apppath;

	ret = rua_init();
	if (ret != 0) {
		tet_infoline("Test program initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}
	ret = rua_add_history(&rec);
	if (ret != 0) {
		tet_infoline("Test program initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}

	ret =
	    rua_delete_history_with_apppath
	    ("/opt/apps/com.samsung.testapp/bin/testapp");
	if (ret == 0) {
		tet_infoline("rua_delete_history_with_apppath test PASS!");
		tet_printf("ret = %d", ret);
		tet_result(TET_PASS);
	} else {
		tet_infoline("rua_delete_history_with_apppath test FAIL!");
		tet_printf("ret = %d", ret);
		tet_result(TET_FAIL);
	}
	return;
}

/**
 * @brief Negative test case of rua_delete_history_with_apppath()
 */
static void utc_rua_delete_history_with_apppath_func_02(void)
{
	int ret = 0;
	ret = rua_delete_history_with_apppath(NULL);
	if (ret == -1) {
		tet_infoline("rua_delete_history_with_apppath test PASS!");
		tet_printf("ret = %d", ret);
		tet_result(TET_PASS);
	} else {
		tet_infoline("rua_delete_history_with_apppath test FAIL!");
		tet_printf("ret = %d", ret);
		tet_result(TET_FAIL);
	}
	return;
}
