


CC                    ?= gcc
AR                    ?= ar
DEFS =
DEFAULT_INCLUDES = -I. -I$(top_builddir)/include
INCLUDES =
AM_CPPFLAGS = -I$(top_srcdir)/include
CFLAGS                = -fPIC -fstack-protector --param=ssp-buffer-size=4 -Wformat -Wformat-security -Wl,-z,relro,-z,now -Wl,--as-needed -Wl,--no-copy-dt-needed-entries -w

IFLAGS                = "-I$(BUILD)/inc"
LDFLAGS               = '-rdynamic' '-Wl,--enable-new-dtags' '-Wl,-rpath,$$ORIGIN/'
LIBPATHS              = -L$(BUILD)/bin
LIBS                  = -lrt -ldl -lpthread -lm

CCLD = $(CC)
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(CFLAGS)
LINK = $(CCLD) $(CFLAGS) $(LDFLAGS) $(AM_LDFLAGS)

WEB_PATH = ~/goahead-4.1.3/build/linux-x86-default/bin

#define the TestEx of exectue
COMM_OBJ = cJSON.o cJSON_utils.o

#define the input file
#cgibin_SOURCES = main.c 
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

define module
	$(LINK) $(1).o $(COMM_OBJ) $(LIBS) -o $(1)
	install -m0777 $(1) $(WEB_PATH)/cgi-bin/
endef


#DEPDIR = .deps
#am__mv = mv -f

all: $(OBJS)
	$(call module, cgi_read_version)
	$(call module, cgi_login)
	$(call module, cgi_read_nodeinfo)
	$(call module, cgi_read_alldev)
 
%.o: %.c
    #$(CC) -c $< -o $@ $(CFLAGS)
	chmod 777 $<
	$(COMPILE) -MD -MP -D_GNU_SOURCE -c -o $@ $<
	#$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
 
clean :
	@rm -rf *.o
 
#.SUFFIXES:
.PHONY : all clean
