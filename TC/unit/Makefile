CC ?= gcc

TARGETS =	utc_ApplicationFW_rua_clear_history_func \
		utc_ApplicationFW_rua_init_func \
		utc_ApplicationFW_rua_fini_func \
		utc_ApplicationFW_rua_delete_history_with_pkgname_func \
		utc_ApplicationFW_rua_add_history_func \
		utc_ApplicationFW_rua_delete_history_with_apppath_func \
		utc_ApplicationFW_rua_is_latest_app_func \
		utc_ApplicationFW_rua_history_load_db_func \
		utc_ApplicationFW_rua_history_unload_db_func \
		utc_ApplicationFW_rua_history_get_rec_func

PKGS = bundle rua

LDFLAGS = `pkg-config --libs $(PKGS)`
LDFLAGS += $(TET_ROOT)/lib/tet3/tcm_s.o
LDFLAGS += -L$(TET_ROOT)/lib/tet3 -ltcm_s
LDFLAGS += -L$(TET_ROOT)/lib/tet3 -lapi_s

CFLAGS = -I. `pkg-config --cflags $(PKGS)`
CFLAGS += -I$(TET_ROOT)/inc/tet3
CFLAGS += -Wall

all: $(TARGETS)

$(TARGETS): %: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGETS)
