# Remove dependancies on system include files
/[^ 	]*$(OBJ)/s,/usr/include/[^ 	]*[ 	]*,,g
/[^ 	]*$(OBJ)/s,/usr/include/[^ 	]*$,,
/[^ 	]*$(OBJ):/s,/usr/local/lib/[^ 	]*[ 	]*,,g
/[^ 	]*$(OBJ):/s,/usr/local/lib/[^ 	]*$,,
/[^	]*$(OBJ):[ 	]*$/d
