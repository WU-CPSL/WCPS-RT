# Wireless Cyber-Physical Simulator - Real-Time (WCPS-RT)
# Yehan Ma yehan.ma@wustl.edu 

For cases of time-driven control (network reconfiguration case of rate adaptation) and self-triggered control

This is a wireless network-in-the-loop implementation integrating load positioning control system simulated in MATLAB/Simulink Desktop Real-time (SLDRT) and contiki implementation of low power wireless bus (LWB) for real TelosB sky motes. 

Note: this is a initial version. We will keep updating it.

There are three parts of codes:

(1) HIL_MATLAB_time_trigger/HIL_MATLAB_self_trigger: SLDRT is used to simulate the physical part of the WNCS: physical plants, controllers, state observers, and physical disturbance. In practice, industrial plants usually operate continuously or at very high rates (delta_r). However, the wireless communication and controller execute at a relatively low rate (delta_t) because of the communication and computation latencies. Therefore, SLDRT modules are operated at different rates in our design.

(2) lwb_time_trigger/lwb_self_trigger: The 3-floor WSAN testbed is deployed on the 3rd to 5th floors of Jolley Hall at Washington University in St. Louis. It consists of 70 TelosB motes. Each mote is equipped with Chipcon CC2420 radio compliant with the IEEE 802.15.4 standard and a TI MSP430 microcontroller. 40 Raspberry Pis with a backplane network are used for the management of the WSAN.

(3) Interfaces: The interfaces between SLDRT and WSAN are  socket connections between the PCs that run SLDRT and the Pis, and serial connections between the Pis and the end nodes.

Please refer to and cite the following article -

Y. Ma, C. Lu and Y. Wang, Efficient Holistic Control: Self-Awareness across Controllers and Wireless Networks, ACM Transactions on Cyber-Physical Systems, Special Issue on Self-Awareness in Resource Constrained Cyber-Physical Systems, 4(4), Article 41, June 2020.

@article{ma2020efficient,
  title={Efficient Holistic Control: Self-Awareness across Controllers and Wireless Networks},
  author={Ma, Yehan and Lu, Chenyang and Wang, Yebin},
  journal={ACM Transactions on Cyber-Physical Systems},
  volume={4},
  number={4},
  pages={1--27},
  year={2020},
  publisher={ACM New York, NY, USA}
}


System architecture:

MATLAB/SLDRT server IP: 172.20.166.113
WASN server IP address to pi: 172.20.166.72
WASN receiver1 (node 108, pi309): 172.20.166.68 
WASN receiver2 (node 112, pi314): 172.20.166.71 
WASN initiator1 (node 110, pi314_2): 172.20.166.70
WASN initiator2 (node 109, pi317): 172.20.166.69


Brief note of the files:

(1) HIL_MATLAB_time_trigger/HIL_MATLAB_self_trigger running on MATLAB/SLDRT server: 

A. Main file of running MATLAB/SDLRT: Linear_system_main.m

B. Simulink (SDTRT) model: Linear_system_mdl.mdl, and the corresponding S-functions in Simulink:
   (a) Plant model: LinearModelSfun1.m
   (b) Controller code in MATLAB: MPCControllersfun.m
   (c) State observer: EKFsfun.m
   (d) Socket communication: SocketHandle.m, ps_aggregation.m

(2) lwb_time_trigger/lwb_self_trigger: 

This is a contiki implementation of LWB for self-triggered control.

We modified the codes of LWB for TelosB: https://github.com/csarkar/tailored-lwb

Please refer to the following article - LWB and FS-LWB implementation for Sky platform using Contiki: https://arxiv.org/pdf/1607.06622.pdf

(3) Interfaces:

Python socket/serial interfaces


Instructions of operating WCPS-RT:

(1) On WASN server (172.20.166.72):

Compile the WSAN:
 
>> /Codes/tailored-lwb-master/tailored-lwb-with-contiki-2.4/apps/tailored-lwb-test$ sudo make TARGET=sky lwb-test.upload
>> sudo msp430-objcopy lwb-test.sky -O ihex lwb-test.ihex
>> python program.py -2 -p /home/yehan/LWB_self_trigger/lwb-test.ihex -a

>> python virtual_server_receiver1_pi320_T.py 
>> python virtual_server_receiver2_pi320_T.py

(2) On WSAN receiver1 (node 108, pi309): 172.20.166.68 

>> python receiver1_pi309.py

on WSAN receiver2 (node 112, pi314): 172.20.166.71 

>> python receiver2_pi314.py

(3) on WSAN initiator1 (node 110, pi314_2):  172.20.166.70

>> python sender1_pi314_2.py

(4) on WSAN initiator2 (node 109, pi317):  172.20.166.69

>> python python sender2_pi317.py

(5) on MATLAB/SLDRT server IP: 172.20.166.113

>> Linear_system_main.m

