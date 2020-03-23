Finished implementing the kernel module (char_dev_kernel_module), and a user space program (user_space_program).
Both these directories include a Makefile, so to compile, just go to the respective directory and fire the command 'make'

Instructions to compile and run:
1. Kernel Module:
Enter the directory char_dev_kernel_module, and type the command: make
Then insert the module using the command: sudo insmod process_list_dev.ko
The kernel module for the character device, named 'process_list_dev' is now inserted to the OS
This module creates a class file for the character device (/sys/class/processlist), and a device node (/dev/processlist)
You can test this module using the 'cat' command as follows:
cat /dev/processlist

2. User Space Program:
Enter the directory user_space_program, and type the command: make
This should create an executable named user_prog, you can execute this as follows:
./user_prog
You should see the output of the character device.