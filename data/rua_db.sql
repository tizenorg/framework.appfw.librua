PRAGMA journal_mode = PERSIST;

CREATE TABLE IF NOT EXISTS rua_history (
	id INTEGER,
	pkg_name TEXT UNIQUE NOT NULL,
	app_path TEXT PRIMARY KEY,
	arg TEXT,
	launch_time INTEGER
);
