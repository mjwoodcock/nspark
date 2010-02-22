# Remove dependancies on system include files
/[^ 	]*\.o:/s,/usr/include/[^ 	]*[ 	]*,,g
/[^ 	]*\.o:/s,/usr/include/[^ 	]*$,,
/[^ 	]*\.o:/s,/usr/local/lib/[^ 	]*[ 	]*,,g
/[^ 	]*\.o:/s,/usr/local/lib/[^ 	]*$,,
/[^	]*\.o:[ 	]*$/d
