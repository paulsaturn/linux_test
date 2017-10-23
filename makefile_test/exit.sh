#!/bin/bash
echo "call extern exit.sh"

#read -p(提示语句) -n(字符个数) -t(等待时间second) -s(不回显)
#当计时满时，read命令返回一个零退出状态
read -n1 -p "Do you want to continue [Y/N]?" answer
#next line
echo ""
case $answer in
Y | y)
     echo "fine,continue"
     ;;
N | n)
     echo "ok,good bye"
	 #Exit to exit.sh and break make here.
	 exit 1     
     ;;
*)
    echo "error choice";;
esac

#Continue next makefile command.
exit 0
