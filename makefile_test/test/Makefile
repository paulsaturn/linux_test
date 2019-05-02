DIR_LIST = $(patsubst %/Makefile,%,$(wildcard */Makefile))
$(info $(DIR_LIST))
MK_LIST := $(addprefix mk-, $(DIR_LIST))
$(info $(MK_LIST))
CLEAN_LIST := $(addprefix clean-, $(DIR_LIST))
$(info $(CLEAN_LIST))

all: $(MK_LIST)
	@echo "[$(realpath ./)]make all"

clean: $(CLEAN_LIST)
	@echo "[$(realpath ./)]make clean"

# make target
define cmd_target_mk
.PHONY : mk-$(1)
mk-$(1) :
	@$(MAKE) -C $(1)
endef

# eval展开
$(foreach mk_file,$(DIR_LIST),$(info $(call cmd_target_mk,$(mk_file))))	
$(foreach mk_file,$(DIR_LIST),$(eval $(call cmd_target_mk,$(mk_file))))	

# clean target
define cmd_target_clean
.PHONY : clean-$(1)
clean-$(1) :
	@$(MAKE) -C $(1) clean
endef

$(foreach clean_file,$(DIR_LIST),$(eval $(call cmd_target_clean,$(clean_file))))	
