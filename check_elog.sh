#!/bin/bash 
HOSTNAME=$1
shift
STATE_OK=0
STATE_CRITICAL=2
OK_MESSAGE="ELOG Daemon is properly serving the page"
CRITICAL_MESSAGE="CRITICAL: ELOG DAEMON IS NOT SERVING THE PAGE"
random_file="$RANDOM""$RANDOM"
wget -e robots=off -O /tmp/$random_file http://$HOSTNAME:8080 -T 30 --tries=2 &> /dev/null
cat /tmp/$random_file | grep -i 'Welcome' &> /dev/null
if [ $? -eq 0 ]; then
    echo $OK_MESSAGE
    echo $STATE_OK
else
    echo $CRITICAL_MESSAGE
    echo $STATE_CRITICAL
fi
rm -f /tmp/$random_file &> /dev/null
