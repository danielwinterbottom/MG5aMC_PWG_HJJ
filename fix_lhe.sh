for i in $(grep -rL "Random number generator exit values" ./*.lhe); do
  last=$(cat $i | grep -n '</event>' | tail -n1 | cut -d: -f1)
  n=$((last+1))
  echo $i $last $n
  range=$(echo $n),\$d
  sed -i $range $i 
done
