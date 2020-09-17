clc;
clear all;
warning off
% slCharacterEncoding('ISO-8859-1')

Round_Count=1;
global delta_t delta_r delta_n 
Tf =200;
delta_t=0.2; %control period 0.2
delta_n=1;
sensor_sampling_rate=1/delta_t;


EMPeriod=1;        %step counter in emergency mode
delta_r=0.01; %discrete rate



delay_s=0;%latency of sensing
delay_a=0;%latency of actuation
delay_KF=0;% latency of observer
delay_MPC=0;% latecy of controller



step_count=Tf*sensor_sampling_rate; %total step count
sen_num = 2; % sensor number 
dtint = 100*delta_t; %

increase_threshold=30;%threshold for sampling rate increase
c=10;
decrease_threshold=increase_threshold/c;%threshold for sampling rate increase increase_threshold/10
increase_interval=150;
decrease_interval=150;
test_period=50;


global raw_X2OUT raw_X1OUT raw_X3OUT raw_X4OUT raw_X5OUT objective traceout actPDR %data collection
raw_X1OUT=[];
raw_X2OUT=[];
raw_X3OUT=[];
raw_X4OUT=[];
raw_X5OUT=[];
objective=[];
traceout= [];
actPDR=[];

%% Load positioning system

mL      = 100;
mB      = 10;
dL      = 15;
kB      = 5;
dB      = 10;


kini      =   [-2 1 2 3];           % Initial stabilziing control policy

A = [0    1             0       0;
    0 -dL/mL-dL/mB   kB/mB   dB/mB;
    0 0                0       1;
    0 dL/mB         -kB/mB  -dB/mB];

B = [0;
    1/mL+1/mB;
    0;
    -1/mB];

C = [1 0 0 0];



mL1      = 15;
mB1      = 5;
dL1      = 10;
kB1      = 2;
dB1      = 3;


A1 = [0    1             0       0;
    0 -dL1/mL1-dL1/mB1   kB1/mB1   dB1/mB1;
    0 0                0       1;
    0 dL1/mB1         -kB1/mB1  -dB1/mB1];

B1 = [0;
    1/mL1+1/mB1;
    0;
    -1/mB1];

C1 = [1 0 0 0];

Xini=[0 3 2 0]';

uini=kini*Xini;

uref=0.2;

Xsp=[0,0,0,0]';

TraceA=ones(1000000,1);
TraceS=ones(1000000,1);

%% RUN Wireless  Process Control Simulation
tic

for k=1:Round_Count
    
global ydelay_global socket_counter socket_counter1 socket_counter2 socket_counter21
ydelay_global = zeros(9,1);
socket_counter = 0;
socket_counter1 = 0;
socket_counter2 = 0;
socket_counter21 = 0;


global ul triggerl seq_nol
ul=0;
triggerl=0; 
seq_nol=0;

global ul1 triggerl1 seq_nol1
ul1=0;
triggerl1=0; 
seq_nol1=0;

global jX bX bP jP bZ Kk Buff Last Buff3 Costfunction triggerS triggerSS triggerT triggerTD triggertimeI triggertimeD triggerB TRACEAA TRACEA_f UU_OUT uu uekf xekf%TRACES_f TRACEA_f TRACEAA;
global jX1 bX1 bP1 jP1 bZ1 Kk1 Buff1 Last1 Buff31 Costfunction1 triggerS1 triggerSS1 triggerT1 triggerTD1 triggertimeI1 triggertimeD1 triggerB1 TRACEAA1 TRACEA_f1 UU_OUT1 uu1 uekf1 xekf1
jX=Xini;
bX=Xini;
bP=eye(4);
jP=eye(4);
bZ=0;
Kk=[0;0;0;0];
Buff=zeros(1,5);
Last=0;
Buff3=zeros(1,5);
uu=[];
uekf=[];
xekf=[];

Costfunction=[];
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
triggerS=1;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
triggerT=0;
triggerTD=0;
triggertimeI=0;
triggertimeD=0;
triggerB=zeros(1,sensor_sampling_rate*Tf+1);
triggerSS=[];
TRACEA_f=zeros(1,(1/delta_r)*Tf+1);
TRACEAA=[];
UU_OUT=[];

jX1=Xini;
bX1=Xini;
bP1=eye(4);
jP1=eye(4);
bZ1=0;
Kk1=[0;0;0;0];
Buff1=zeros(1,5);
Last1=0;
Buff31=zeros(1,5);
uu1=[];
uekf1=[];
xekf1=[];

Costfunction1=[];
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
triggerS1=1;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
triggerT1=0;
triggerTD1=0;
triggertimeI1=0;
triggertimeD1=0;
triggerB1=zeros(1,sensor_sampling_rate*Tf+1);
triggerSS1=[];
TRACEA_f1=zeros(1,(1/delta_r)*Tf+1);
TRACEAA1=[];
UU_OUT1=[];


Ydelay = zeros(step_count+1,sen_num);
Delay = 0;
ranD=[4 2 2 2 2]; %delayed time steps 

    
    delay1 = zeros(step_count+1,sen_num+1);

    yin=0;
    yin_d=0;
    ystore = Ydelay;
    count = 1;
    i = 0;

    last_emergency_instance=[0 0 0 0 0];
    RetransCounter=[0 0 0 0 0];

    
    option = simset('solver','ode4','FixedStep',delta_t);
    sim('Linear_system_mdl.mdl',[0 Tf]);

end


toc



