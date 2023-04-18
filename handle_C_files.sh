if ! test $# -eq 2
then
    echo "Usage <file> <file_result>";exit 1
fi

if ! test -f $1  ||  test -h $1
then
    echo "Usage <file><file>";exit 1
fi
if ! test -f $2  ||  test -h $2
then
    echo "Usage <file>2";exit 1
fi
searched1='error:'
searched2='warning:'

gcc -Wall $1 2> $2
count_err=`cat $2 | grep -E -c "$searched1"`
count_wrn=`cat $2 | grep  -E -c "$searched2"`
echo " counter_err=$count_err"
echo " counter_war=$count_wrn"
exit(count_err+count_wrn)