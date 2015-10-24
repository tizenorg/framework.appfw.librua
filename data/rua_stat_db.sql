PRAGMA journal_mode = PERSIST;

CREATE TABLE IF NOT EXISTS rua_panel_stat (
	    caller_panel TEXT NOT NULL,
	    rua_stat_tag TEXT NOT NULL,
	    score INTEGER DEFAULT 0,
	    PRIMARY KEY (rua_stat_tag , caller_panel)
);

