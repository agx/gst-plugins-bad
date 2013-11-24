plugin_LTLIBRARIES = libgstbcmdec.la

libgstbcmdec_la_SOURCES = gstbcmdec.c decif.c parse.c
libgstbcmdec_la_CFLAGS = $(GST_PLUGINS_BAD_CFLAGS) $(GST_CFLAGS) \
	$(BCMDEC_CFLAGS) $(CRYSTALHD_CFLAGS)
libgstbcmdec_la_LIBADD = $(GST_LIBS) $(GST_PLUGINS_BASE_LIBS) \
	-lgstvideo-$(GST_API_VERSION) $(CRYSTALHD_LIBS)
libgstbcmdec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstbcmdev_la_LIBTOOLFLAGS = $(GST_PLUGIN_LIBTOOLFLAGS)

noinst_HEADERS = gstbcmdec.h decif.h parse.h
