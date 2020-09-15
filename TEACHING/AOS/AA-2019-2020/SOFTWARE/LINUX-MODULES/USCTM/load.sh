./decompress.sh $1 >  linux-img
text=`objdump -h -j .text -j .rodata linux-img | grep "text \|rodata " |  tr -s ' ' | cut -d' ' -f5 | tr 'a-z' 'A-Z'`
rodata=`echo $text | cut -d' ' -f2`
text=`echo $text | cut -d' ' -f1`
echo TEXT:$text
echo RODATA:$rodata
offset=`echo "obase=16;ibase=16;$rodata - $text" | bc `
echo RODATA_TO_TEXT_OFFSET:0x$offset
echo "running command: sudo insmod the_usctm.ko myoffset=0x$offset"
sudo insmod the_usctm.ko myoffset=0x$offset
