# Access Control 

Name:  Soklong Lim
SID: 11458249
Email:  soklong.lim@wsu.edu
CS483 (Fall 2018) 
Washington State University, Vancouver

Description:
  The UNIX file system protection scheme does not allow for fine-grained 
control over access using only the standard user(u), group(g), and others(o) 
protection bits. For example, its not possible using only these bits to grant 
one set of users read access, a second set of users write access, and a third 
set of users both read and write access. Further, while the group bits may 
suffice, using these access bits typically requires intervention by the super-user. 
However, the SETUID and SETGID bits can sometimes be used to provide finer-grained 
control when it is desired. In this assignment, you will create a “get” command that 
allow a user to provide finer-grained control over access to their files without the 
intervention of the super-user.

# Build and Run
$ make
$ ./get <source> <destination>

# File in arhives
README.txt ........... This file.
Make ................. Source code builder.
get.h ................ Heaher file.
get.c ................ Source file.
