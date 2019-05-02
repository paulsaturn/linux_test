#MAKEFILE_LIST: 只包括include的嵌套的Makefile,而用make命令的MAKEFILE_LIST会重置
all : 
	@echo "-> $(MAKEFILE_LIST)";
	@echo $(realpath $(dir $(lastword $(MAKEFILE_LIST))));
	@echo "[$(realpath ./)]make all";
	
clean : 
	@echo "[$(realpath ./)]make clean";
