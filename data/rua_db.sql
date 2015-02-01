PRAGMA journal_mode = PERSIST;

CREATE TABLE IF NOT EXISTS rua_history (
	id INTEGER,
	pkg_name TEXT PRIMARY KEY,
	app_path TEXT,
	arg TEXT,
	launch_time INTEGER
);
