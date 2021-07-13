


CC                    ?= gcc
AR                    ?= ar
DEFS =
DEFAULT_INCLUDES = -I. -I$(top_builddir)/include -I ../uci
INCLUDES =
AM_CPPFLAGS = -I$(top_srcdir)/include
CFLAGS                = -fPIC -fstack-protector --param=ssp-buffer-size=4 -Wformat -Wformat-security -Wl,-z,relro,-z,now -Wl,--as-needed -Wl,--no-copy-dt-needed-entries -w

IFLAGS                = "-I$(BUILD)/inc"
LDFLAGS               = '-rdynamic' '-Wl,--enable-new-dtags' '-Wl,-rpath,$$ORIGIN/'
LIBPATHS              = -L/home/kingw/code/test/uci -luci
LIBS                  = -L/usr/lib/x86_64-linux-gnu -lrt -ldl -lpthread -lm

CCLD = $(CC)
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(CFLAGS)
LINK = $(CCLD) $(CFLAGS) $(LDFLAGS) $(AM_LDFLAGS)

WEB_PATH = ~/goahead-4.1.3/build/linux-x86-default/bin

#define the TestEx of exectue
COMM_OBJ = cJSON.o cJSON_utils.o cgi_common.o cgi_uci_common.o

#define the input file
#cgibin_SOURCES = main.c 
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

define module
	$(LINK) $(1).o $(COMM_OBJ) $(LIBPATHS) $(LIBS) -o bin/$(1)
	install -m0777 bin/$(1) $(WEB_PATH)/cgi-bin/
endef


#DEPDIR = .deps
#am__mv = mv -f

all: $(OBJS)
	mkdir -p bin
	mkdir -p debug
	$(call module,cgi_login)
	$(call module,cgi_read_nodeinfo)
	$(call module,cgi_read_alldev)
	$(call module,cgi_read_topology)
	mv *.o debug/
	mv *.d debug/
 
%.o: %.c
    #$(CC) -c $< -o $@ $(CFLAGS)
	chmod 777 $<
	$(COMPILE) -MD -MP -D_GNU_SOURCE -c -o $@ $<
	#$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
 
clean :
	@rm -rf *.o *.d bin/ debug/
 
#.SUFFIXES:
.PHONY : all clean
