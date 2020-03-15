for i in `ls levels | grep -v list`
do
  sed -i '' "s/\]$/\], \"$1\": \[\]/" levels/$i
done
