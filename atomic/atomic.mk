#=========================================================================
TARGETS := $(TARGETS_DIR)/test_android_atomic
LIB_TARGETS := libandroid_atomic.a

SRCS_C := test_android_atomic.c
OBJS_C := $(subst .c,.o,$(SRCS_C))
OBJS_C := $(foreach i,$(OBJS_C),$(TARGETS_OBJ_DIR)/$(i))

OBJS := $(OBJS_C) $(OBJS_CXX)

LIB_SRCS_C := atomic.c
LIB_OBJS_C := $(subst .c,.o,$(LIB_SRCS_C))
LIB_OBJS_C := $(foreach i,$(LIB_OBJS_C),$(TARGETS_OBJ_DIR)/$(i))

LIB_OBJS := $(LIB_OBJS_C) $(LIB_OBJS_CXX)

#CFLAGS := -DANDROID_SMP=0
CFLAGS := -DANDROID_SMP=1
CFLAGS += -fPIC
#=========================================================================	
LOCAL_LDLIBS := -lm -lpthread

#=========================================================================	
$(TARGETS): $(OBJS) $(LIB_TARGETS)
	@echo "[LD] $(TARGETS)"
	$(_Q)$(CC) $(LDFLAGS) -o $@ $^ $(LOCAL_LDLIBS)	

$(LIB_TARGETS): $(LIB_OBJS) 
	@echo "[AR] $@"
	$(_Q)$(AR) rcs $@ $^ 	
#=========================================================================	
include $(TARGETS_ROOT)/mkconfig	
