make 2>&1 | grep "undefined reference" | sed -e "s/^.*undefined reference to\(.*\)/\1/" | sort | uniq -c | sort -n -r
