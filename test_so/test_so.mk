#=========================================================================
#TARGETS_C := $(TARGETS_DIR)/pulse-test
TARGETS := $(TARGETS_DIR)/test_so
TARGETS_LIB := $(TARGETS_DIR)/libtest.so
TARGETS_LIB1 := $(TARGETS_DIR)/libtest1.so

TARGETS_CLEAN := $(TARGETS) $(TARGETS_LIB) $(TARGETS_LIB1)

SRCS_C := test_so.c
OBJS_C := $(subst .c,.o,$(SRCS_C))
OBJS_C := $(foreach i,$(OBJS_C),$(TARGETS_OBJ_DIR)/$(i))
OBJS := $(OBJS_C) $(OBJS_CXX)

LIB_SRCS_CXX := libtest_so.cpp
LIB_OBJS_CXX := $(subst .cpp,.o,$(LIB_SRCS_CXX))
LIB_OBJS_CXX := $(foreach i,$(LIB_OBJS_CXX),$(TARGETS_OBJ_DIR)/$(i))

LIB_SRCS_C := libtest1_so.c
LIB_OBJS_C := $(subst .c,.o,$(LIB_SRCS_C))
LIB_OBJS_C := $(foreach i,$(LIB_OBJS_C),$(TARGETS_OBJ_DIR)/$(i))

#=========================================================================	
CFLAGS += -fPIC
CFLAGS += -I$(TARGETS_ROOT)/atomic

LOCAL_LDLIBS := -ldl
LOCAL_LIB_LDLIBS := $(TARGETS_ROOT)/atomic/libandroid_atomic.a

LOCAL_LDFLAGS += -Wl,--wrap,malloc
LOCAL_LDFLAGS += -Wl,--wrap,free
LOCAL_LDFLAGS += -Wl,--wrap,calloc
LOCAL_LDFLAGS += -Wl,--wrap,realloc

LOCAL_LDFLAGS += -Wl,--wrap,_Znwj
LOCAL_LDFLAGS += -Wl,--wrap,_Znwm
LOCAL_LDFLAGS += -Wl,--wrap,_ZdlPv
LOCAL_LDFLAGS += -Wl,--wrap,_Znaj
LOCAL_LDFLAGS += -Wl,--wrap,_Znam
LOCAL_LDFLAGS += -Wl,--wrap,_ZdaPv

LOCAL_LDFLAGS += -Wl,--wrap,strndup
LOCAL_LDFLAGS += -Wl,--wrap,strdup
LOCAL_LDFLAGS += -Wl,--wrap,__strdup

LIB_LDFLAGS := $(LOCAL_LDFLAGS)
#=========================================================================	
$(TARGETS): $(TARGETS_LIB) $(TARGETS_LIB1) $(OBJS)
	@echo "[LD] $(TARGETS)"
	$(_Q)$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LOCAL_LDLIBS)
	
$(TARGETS_LIB):	$(LIB_OBJS_CXX) $(TARGETS_OBJ_DIR)/wrap_malloc.o
	@echo "[LD] $(TARGETS_LIB)"
	$(_Q)$(CXX) -shared $(LIB_LDFLAGS) $(LDFLAGS) -o $@ $^ $(LOCAL_LIB_LDLIBS)

$(TARGETS_LIB1): $(LIB_OBJS_C) $(TARGETS_OBJ_DIR)/wrap_malloc.o
	@echo "[LD] $(TARGETS_LIB1)"
	$(_Q)$(CC) -shared $(LIB_LDFLAGS) $(LDFLAGS) -o $@ $^ $(LOCAL_LIB_LDLIBS)
	
#=========================================================================	
include $(TARGETS_ROOT)/mkconfig	
