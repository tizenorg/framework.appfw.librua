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

static void utc_rua_history_get_rec_func_01(void);
static void utc_rua_history_get_rec_func_02(void);

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

struct tet_testlist tet_testlist[] = {
	{utc_rua_history_get_rec_func_01, POSITIVE_TC_IDX},
	{utc_rua_history_get_rec_func_02, NEGATIVE_TC_IDX},
	{NULL, 0}
};

static void startup(void)
{
	tet_infoline("Testing API rua_history_get_rec start");
	return;
}

static void cleanup(void)
{
	tet_infoline("Testing API rua_history_get_rec ends ");
	rua_clear_history();
	rua_fini();
	return;
}

/**
 * @brief Positive test case of rua_history_get_rec()
 */
static void utc_rua_history_get_rec_func_01(void)
{
	struct rua_rec rec_result = { 0, };
	int ret = 0;
	char **table = NULL;
	int nrows = 0;
	int ncols = 0;
	int row = 0;

	struct rua_rec rec = { 0, };
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
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}

	ret = rua_history_load_db(&table, &nrows, &ncols);
	if (ret != 0) {
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}
	ret = rua_history_get_rec(&rec_result, table, nrows, ncols, row);
	if (ret == 0) {
		tet_infoline("rua_history_get_rec test PASS!");
		tet_printf("ret = %d", ret);
		tet_result(TET_PASS);
	} else {
		tet_infoline("rua_history_get_rec test FAIL!");
		tet_printf("ret = %d", ret);
		tet_result(TET_FAIL);
	}
	rua_history_unload_db(&table);
	return;
}

/**
 * @brief Negative test case of rua_history_get_rec()
 */
static void utc_rua_history_get_rec_func_02(void)
{
	int ret = 0;
	struct rua_rec rec_result = { 0, };
	char **table = NULL;
	int nrows = 0;
	int ncols = 0;
	int row = 0;

	struct rua_rec rec = { 0, };
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
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}

	ret = rua_history_load_db(&table, &nrows, &ncols);
	if (ret != 0) {
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}
	ret = rua_history_get_rec(&rec_result, table, nrows, ncols, row + 10);
	if (ret == -1) {
		tet_infoline("rua_history_get_rec test PASS!");
		tet_printf("ret = %d", ret);
		tet_result(TET_PASS);
	} else {
		tet_infoline("rua_history_get_rec test FAIL!");
		tet_printf("ret = %d", ret);
		tet_result(TET_FAIL);
	}
	rua_history_unload_db(&table);
	return;
}
