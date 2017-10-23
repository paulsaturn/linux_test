#!/bin/bash  
#赋值语句，不加空格
count=1   
#cat 命令的输出作为read命令的输入,read读到的值放在line中
cat testset.sh | while read line       
do
  echo "Line $count:$line"
  #注意中括号中的空格。
  count=$[ $count +1 ]          
done

echo "finish"
exit 0
