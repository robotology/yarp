
for f in `find . -iname "Makefile.man"`; do
    echo $f
    svn rm $f
done
