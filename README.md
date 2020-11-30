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
5. Package previous project as an IP in GUI. Associate "ddr" interface to "ddr_clk".
