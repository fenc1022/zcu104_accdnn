# zcu104_accdnn

The code in this repo is used to run a complete test benchmark on Xilinx ZCU104 board.  
This repo depends on the output of repo AccDNN.

In AccDNN directory:

1. Generate HDL source by    
    `python ./codegen.py example/cifar10/cifar10_quick.prototxt example/cifar10/cifar10_quick_iter_5000.caffemodel --optim_file example/cifar10/optim_cifar10.conf`  
2. Generate elementary IP by  
    `vivado -mode batch -source ./build/ips.tcl`  
3. Generate additional HDL by  
    `./binsim_file_gen.sh`   
4. Generate complete accelerator as a Vivado project by
    `cd ./lib/ip`  
    `vivado -mode batch -source ips.tcl`  
5. Package previous project as an IP in GUI. Here, we need to    
   -- Associate "ddr" interface to "ddr_clk";    
   -- edit port "complete_int", set SENSITIVITY to EDGE RISING;
   -- edit port clk & ddr_clk, set FREQ_HZ to user defined.

In zcu104_accdnn directory:

1. Generate Vivado project by    
    `vivado -mode batch -source ./zcu104_base.tcl`   
2. Check project design and generate bitstream in Vivado GUI, then export .XSA file in File/export/export hardware/fixed/include bitstream menu.   
3. petalinux setup    
    `petalinux-create -t project -n prj_name --template zynqMP`(if clone plnx_prj from git, no need to do this step)    
    `petalinux-config --get-hw-description <PATH TO XSA FILE>`    
    `petalinux-build`    
4. package booting files    
    `petalinux-package --boot --fpga --u-boot --force`   
5. prepare the boot SD card(first partition 100M FAT32, second partition EXT4)   
    `cp ~/zcu104_accdnn/plnx_prj/images/linux/BOOT.BIN <path to first partition on SD card>`   
    `cp ~/zcu104_accdnn/plnx_prj/images/linux/boot.scr <path to first partition on SD card>`   
    `cp ~/zcu104_accdnn/plnx_prj/images/linux/image.ub <path to first partition on SD card>`   
    `sudo tar xvf ~/zcu104_accdnn/plnx_prj/images/linux/rootfs.tar.gz <path to 2nd partition on SD card>`  
   then this SD card can be used to boot ZCU104 board



