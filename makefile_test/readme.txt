1. 如果把all clean:这一段mask掉，改为include mk1/Android.mk的输出：
======== /home/paul2/work/test/linux_test/makefile_test/mk1/Android.mk =======
======== /home/paul2/work/test/linux_test/makefile_test/mk1/Android.mk =======
Android.mk mk1/Android.mk
mk1

那么现在$(MAKEFILE_LIST), 为Android.mk mk1/Android.mk, 而当前目录为/home/paul2/work/test/linux_test/makefile_test，
$(lastword $(MAKEFILE_LIST))为mk1/Android.mk

2. eval函数
  函数原型 $(eval text)
	它的意思是text的内容将作为makefile的一部分而被make解析和执行
	函数“eval”是一个比较特殊的函数。使用它我们可以在我们的Makefile中构造一个可变的规则结构关系（依赖关系链），其中可以使用其它变量和函数。
	函数“eval”对它的参数进行展开，展开的结果作为Makefile的一部分，make可以对展开内容进行语法解析。展开的结果可以包含一个新变量、目标、隐含规则或者是明确规则等。
	也就是说此函数的功能主要是：根据其参数的关系、结构，对它们进行替换展开.
	
	做两次变量展开；以$(eval var),首先会对var做一次变量展开，然后对展开后的结果，再执行一次变量展开
	
	1). 
	$(eval xd:xd.c a.c)

	将会产生一个这样的编译
	cc   xd.c a.c -o xd

	2).
	define MA
	aa:aa.c
	 gcc  -g -o aa aa.c
	endef 

	$(eval $(call MA) )
	
	会产生一个这样的编译：
	gcc -g -o aa aa.c

	3).
	OBJ=a.o b.o c.o d.o main.o

	define MA
	main:$(OBJ)
	 gcc  -g -o main $$(OBJ)
	endef 

	$(eval $(call MA) )

	会产生这样的编译过程：
	cc -c -o a.o a.c
	cc -c -o b.o b.c
	cc -c -o c.o c.c
	g++ -c -o d.o d.cpp
	cc -c -o main.o main.c
	gcc -g -o main a.o b.o c.o d.o main.o

	请注意到$$(OBJ) ,因为make要把这个作为makefile的一行，要让这个地方出现$,就要用两个$,因为两个$,make才把作为$字符。
	
	(1)
	从eval目录下的Makefile编译可以看出, eval函数在读入makefile的时就展开解析, 所以出现
	server_OBJS = .obj/server.o .obj/server_priv.o .obj/server_access.o
	ALL_OBJS = .obj/server.o .obj/server_priv.o .obj/server_access.o .obj/client.o .obj/client_api.o .obj/client_mem.o
	client_OBJS = .obj/client.o .obj/client_api.o .obj/client_mem.o
	ALL_OBJS = .obj/server.o .obj/server_priv.o .obj/server_access.o .obj/client.o .obj/client_api.o .obj/client_mem.o
	
	(2).在ALL_OBJS += $$($(notdir $(1))_OBJS)下面增加$(info $(ALL_OBJS))
	输出：
	.obj/server

	.obj/client
	.obj/server.o .obj/server_priv.o .obj/server_access.o
	
	###
	1st time replacement, 'PROGRAM_template' is executed as shell:  
	2nd time replacement, execute as makefile:
	###
		
	如果把
	ALL_OBJS += $$($(notdir $(1))_OBJS)
	改为
	$(eval ALL_OBJS += $$($(notdir $(1))_OBJS))
	输出：
	.obj/server
	.obj/server.o .obj/server_priv.o .obj/server_access.o
	.obj/client
	.obj/server.o .obj/server_priv.o .obj/server_access.o .obj/client.o .obj/client_api.o .obj/client_mem.o

  4).
	看下面这个例子：
        define import_target
          include $(1)
          _PREFIXID := $(if $2, $2, TARGET)
          $(_ PREFIXID)
        endef

	按上面定义的宏，当去计算 a := $(call import_target)时，几乎总是会报错。
	原因是宏定义只是简单的做字符串替换,一经替换后，原来看起来是一行语句，一下子就变成多行，从而导致makefile解析错误。
	于是只能使用“\”将各个语句连接为一行。
        define import_target
          include $(1) \
           _PREFIXID := $(if $2, $2,TARGET) \
          $(_PREFIXID)
        endef
	这样做相当于
         include $(1) _PREFIXID := $(if $2, $2, TARGET) $(_PREFIXID)

	显然，肯定还是报解析错误。
	最终解决方案，将各个子语句使用$(eval )包裹，
	define import_target
		$(eval include $(1)) \
		$(eval _PREFIXID := $(if $2,$2, TARGET)) \
		$(_PREFIXID)
	endef 
	
	解析时，makefile先对$(1)做展开，假设结果为xxx，这是第一次；然后执行include xxx，这是第二次展开。
	执行完后，整个$(eval include $(1))表达式返回值为空。这样解析错误解决了，
	而且import_target的返回结果又正好是_PREFIXID的值。	
			
3. make -d或make --debug=<opt> 来查看makefile的处理过程
	opt : 
		b for basic debugging, 
		v for more verbose basic debugging, 
		i for showing implicit rules, 
		j for details on invocation of commands, 
		m for debugging while remaking makefiles
		p for Print the data base (rules and variable values) that results from reading the makefiles; 
			  then execute as usual or as otherwise specified 
		t for Touch files (mark them up to date without really changing them) instead of running their commands
		n for Print the commands that would be executed, but do not execute them	  
			  
4.
  MAKEFILE_LIST
	make 程序在读取多个 makefile 文件时,包括由环境变量“MAKEFILES”指定, 命令行指定, 当前工作下的默认的以及使用指示符“include”指定包含的,
	在对这些文件进行解析执行之前make读取的文件名将会被自动依次追加到变量“MAKEFILE_LIST”的定义域中。
	这样我们就可以通过测试此变量的最后一个字来获取当前 make 程序正在处理的makefile文件名。具体地说就是在一个makefile文件中如果使用指示符“include”包
	含另外一个文件之后, “MAKEFILE_LIST”变量的最后一个字只可能是指示符“include”指定所要包含的那个文件的名字。一个 makefile 的内容如下:
	
	name1 := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))	
  
  MAKELEVEL	
	变量“MAKELEVEL”代表了调用的深度
	
5. 在include <file>时, 如果makefile中有目标<file>， 就会执行目标<file>				

6. install命令
--backup[=CONTROL]: 为每个已存在的目的地文件进行备份。
-b: 类似--backup，但不接受任何参数。
-c: (此选项不作处理)。
-d: --directory：所有参数都作为目录处理，而且会创建指定目录的所有主目录。
-D: 创建<目的地>前的所有主目录，然后将<来源>复制至 <目的地>；在第一种使用格式中有用。
-g: --group=组：自行设定所属组，而不是进程目前的所属组。
-m: --mode=模式：自行设定权限模式 (像chmod)，而不是rwxr－xr－x。
-o: --owner=所有者：自行设定所有者 (只适用于超级用户)。
-p: --preserve－timestamps：以<来源>文件的访问/修改时间作为相应的目的地文件的时间属性。
-s: --strip：用strip命令删除symbol table，只适用于第一及第二种使用格式。
-S: --suffix=后缀：自行指定备份文件的<后缀>。
-v: --verbose：处理每个文件/目录时印出名称。
--help: 显示此帮助信息并离开。
--version: 显示版本信息并离开。 
