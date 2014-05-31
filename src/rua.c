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

/*
 * @file    rua.c
 * @author  Noha Park (noha.park@samsung.com)
 * @version 0.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <db-util.h>

#include "rua.h"
#include "db-schema.h"
#include "perf-measure.h"

#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "RUA"

#define RUA_DB_PATH	"/opt/dbspace"
#define RUA_DB_NAME	".rua.db"
#define RUA_HISTORY	"rua_history"
#define QUERY_MAXLEN	4096
#define Q_LATEST \
	"select pkg_name from rua_history " \
	"order by launch_time desc limit 1 "

static sqlite3 *_db = NULL;
static int __exec(sqlite3 *db, char *query);
static sqlite3 *__db_init(char *root);

#define _F(fmt, arg...) do { \
	FILE *fp; \
	fp = fopen("/opt/usr/media/.rua.log", "a+"); \
	if (NULL == fp) break; \
	fprintf(fp, "[%d]"fmt"\n", getpid(),##arg); \
	fclose(fp); \
} while (0)

int rua_clear_history(void)
{
	int r;
	char query[QUERY_MAXLEN];

	if (_db == NULL)
		return -1;

	snprintf(query, QUERY_MAXLEN, "delete from %s;", RUA_HISTORY);

	r = __exec(_db, query);
	if (r == -1) {
		LOGE("rua_clear_history error");
		return -1;
	}

	return r;
}

int rua_delete_history_with_pkgname(char *pkg_name)
{
	int r;
	char* query = NULL;

	if (_db == NULL)
		return -1;

	if (pkg_name == NULL)
		return -1;

	query = sqlite3_mprintf("delete from %s where pkg_name = %Q;",
		RUA_HISTORY, pkg_name);

	r = __exec(_db, query);
	if (r == -1) {
		LOGE("rua_delete_history_with_pkgname error");
		sqlite3_free(query);
		return -1;
	}

	sqlite3_free(query);

	return r;
}

int rua_delete_history_with_apppath(char *app_path)
{
	int r;
	char* query=NULL;

	if (_db == NULL)
		return -1;

	if (app_path == NULL)
		return -1;

	query = sqlite3_mprintf("delete from %s where app_path = %Q;",
		RUA_HISTORY, app_path);

	r = __exec(_db, query);
	if (r == -1) {
		LOGE("rua_delete_history_with_apppath error");
		sqlite3_free(query);
		return -1;
	}

	sqlite3_free(query);

	return r;
}

int rua_add_history(struct rua_rec *rec)
{
	int r;
	int cnt = 0;
	char* query=NULL;
	sqlite3_stmt *stmt;

	unsigned int timestamp;
	timestamp = PERF_MEASURE_START("RUA");

	if (_db == NULL)
		return -1;

	if (rec == NULL)
		return -1;

	query = sqlite3_mprintf(
		"select count(*) from %s where pkg_name = %Q;", RUA_HISTORY,
		rec->pkg_name);

	r = sqlite3_prepare(_db, query, strlen(query), &stmt, NULL);
	if (r != SQLITE_OK) {
		sqlite3_free(query);
		LOGE("sqlite3_prepare error(%d)", r);
		return -1;
	}

	sqlite3_free(query);
	query = NULL;

	r = sqlite3_step(stmt);
	if (r == SQLITE_ROW) {
		cnt = sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);

	if (cnt == 0)
		/* insert */
		query = sqlite3_mprintf(
			"insert into %s ( pkg_name, app_path, arg, launch_time ) "
			" values ( %Q, %Q, %Q, %d ) ",
			RUA_HISTORY,
			rec->pkg_name ? rec->pkg_name : "",
			rec->app_path ? rec->app_path : "",
			rec->arg ? rec->arg : "", (int)time(NULL));
	else
		/* update */
		query = sqlite3_mprintf(
			"update %s set arg=%Q, launch_time='%d' where pkg_name = %Q;",
			RUA_HISTORY,
			rec->arg ? rec->arg : "", (int)time(NULL), rec->pkg_name);

	r = __exec(_db, query);
	if (r == -1) {
		sqlite3_free(query);
		LOGE("rua_add_history error");
		return -1;
	}

	sqlite3_free(query);

	PERF_MEASURE_END("RUA", timestamp);

	return r;
}

int rua_history_load_db(char ***table, int *nrows, int *ncols)
{
	int r;
	char query[QUERY_MAXLEN];
	char *db_err = NULL;
	char **db_result = NULL;

	if (table == NULL)
		return -1;
	if (nrows == NULL)
		return -1;
	if (ncols == NULL)
		return -1;

	snprintf(query, QUERY_MAXLEN,
		 "select * from %s order by launch_time desc;", RUA_HISTORY);

	r = sqlite3_get_table(_db, query, &db_result, nrows, ncols, &db_err);
	if (r == SQLITE_OK) {
		*table = db_result;
		LOGE("rua_history_load_db ok. nrows : %d, ncols : %d", *nrows, *ncols);
	} else {
		SECURE_LOGE("rua_history_load_db error(%d / %s)", r, db_err);
		sqlite3_free_table(db_result);
	}

	return r;
}

int rua_history_unload_db(char ***table)
{
	if (*table) {
		sqlite3_free_table(*table);
		*table = NULL;
		return 0;
	}
	return -1;
}

int rua_history_get_rec(struct rua_rec *rec, char **table, int nrows, int ncols,
			int row)
{
	char **db_result = NULL;
	char *tmp = NULL;

	if (rec == NULL)
		return -1;
	if (table == NULL)
		return -1;
	if (row >= nrows)
		return -1;

	db_result = table + ((row + 1) * ncols);

	tmp = db_result[RUA_COL_ID];
	if (tmp) {
		rec->id = atoi(tmp);
	}

	tmp = db_result[RUA_COL_PKGNAME];
	if (tmp) {
		rec->pkg_name = tmp;
	}

	tmp = db_result[RUA_COL_APPPATH];
	if (tmp) {
		rec->app_path = tmp;
	}

	tmp = db_result[RUA_COL_ARG];
	if (tmp) {
		rec->arg = tmp;
	}

	tmp = db_result[RUA_COL_LAUNCHTIME];
	if (tmp) {
		rec->launch_time = atoi(tmp);
	}

	return 0;
}

int rua_is_latest_app(const char *pkg_name)
{
	int r;
	sqlite3_stmt *stmt;
	const unsigned char *ct;

	if (!pkg_name || !_db)
		return -1;

	r = sqlite3_prepare(_db, Q_LATEST, sizeof(Q_LATEST), &stmt, NULL);
	if (r != SQLITE_OK) {
		LOGE("rua_is_latest_app prepare error(%d)", r);
		return -1;
	}

	r = sqlite3_step(stmt);
	if (r == SQLITE_ROW) {
		ct = sqlite3_column_text(stmt, 0);
		if (ct == NULL || ct[0] == '\0') {
			LOGW("sqlite3_column_text null");
			sqlite3_finalize(stmt);
			return -1;
		}

		if (strncmp(pkg_name, (const char*)ct, strlen(pkg_name)) == 0) {
			SECURE_LOGW("pkg_name(%s) / ct(%s)", pkg_name, ct);
			sqlite3_finalize(stmt);
			return 0;
		}
	}

	sqlite3_finalize(stmt);
	return -1;
}

int rua_init(void)
{
	unsigned int timestamp;
	timestamp = PERF_MEASURE_START("RUA");

	if (_db) {
		return 0;
	}

	char defname[FILENAME_MAX];
	snprintf(defname, sizeof(defname), "%s/%s", RUA_DB_PATH, RUA_DB_NAME);
	_db = __db_init(defname);

	if (_db == NULL) {
		LOGW("rua_init error");
		return -1;
	}

	PERF_MEASURE_END("RUA", timestamp);

	return 0;
}

int rua_fini(void)
{
	unsigned int timestamp;
	timestamp = PERF_MEASURE_START("RUA");

	if (_db) {
		db_util_close(_db);
		_db = NULL;
	}

	PERF_MEASURE_END("RUA", timestamp);
	return 0;
}

static int __exec(sqlite3 *db, char *query)
{
	int r;
	char *errmsg = NULL;

	if (db == NULL)
		return -1;

	r = sqlite3_exec(db, query, NULL, NULL, &errmsg);
	if (r != SQLITE_OK) {
		SECURE_LOGE("query(%s) exec error(%s)", query, errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	return 0;
}

static int __create_table(sqlite3 *db)
{
	int r;

	r = __exec(db, CREATE_RUA_HISTORY_TABLE);
	if (r == -1) {
		LOGE("create table error");
		return -1;
	}

	return 0;
}

static sqlite3 *__db_init(char *root)
{
	int r;
	sqlite3 *db = NULL;

	r = db_util_open(root, &db, 0);
	if (r) {
		_F("db util open error(%d/%d/%d/%s)", r,
			sqlite3_errcode(db),
			sqlite3_extended_errcode(db),
			sqlite3_errmsg(db));
		return NULL;
	}

	return db;
}
