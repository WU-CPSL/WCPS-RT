function [sys,x0,str,ts,simStateCompliance] = LinearModelSfun1(t,x,u,flag,A,B,C,Xini,delta_t,delta_r,delay_s,delay_a,delay_KF,delay_MPC)


switch flag,

  case 0,
    [sys,x0,str,ts,simStateCompliance]=mdlInitializeSizes(Xini);

  case 1,
    sys=mdlDerivatives(t,x,u);

  case 2,
    sys=mdlUpdate(t,x,u,A,B,C,delta_t,delta_r,delay_s,delay_a,delay_KF,delay_MPC);

  case 3,
    sys=mdlOutputs(t,x,u,C);

  case 4,
    sys=mdlGetTimeOfNextVarHit(t,x,u);

  case 9,
    sys=mdlTerminate(t,x,u);

  otherwise
    DAStudio.error('Simulink:blocks:unhandledFlag', num2str(flag));

end



function [sys,x0,str,ts,simStateCompliance]=mdlInitializeSizes(Xini)


sizes = simsizes;

sizes.NumContStates  = 0;   
sizes.NumDiscStates  = 4;   %x1, x2, x3, x4
sizes.NumOutputs     = 5;   %x1, x2,x3, x4, y
sizes.NumInputs      = 2;   %u
sizes.DirFeedthrough = 1;
sizes.NumSampleTimes = 1;   % at least one sample time is needed

sys = simsizes(sizes);

x0  = Xini;  

str = [];

ts  = [0 0];

simStateCompliance = 'UnknownSimState';



function sys=mdlDerivatives(t,x,u)

sys=[];



function sys=mdlUpdate(t,x,u,A,B,C,delta_t,delta_r,delay_s,delay_a,delay_KF,delay_MPC)
global Buff31 TRACEAA1 uu1;
Atrace=u(1);
Buff4=[Buff31(1)*ones(1,delta_t/delta_r) Buff31(2)*ones(1,delta_t/delta_r) Buff31(3)*ones(1,delta_t/delta_r) Buff31(4)*ones(1,delta_t/delta_r) Buff31(5)*ones(1,delta_t/delta_r)];
TRACEAA1=[TRACEAA1 Atrace];
SSMA=delta_r*A+eye(4);
SSMB=delta_r*B;
ddd=SSMA*x+SSMB*u(2);
uu1=[uu1;u(2)];
sys = ddd;



function sys=mdlOutputs(t,x,u,C)
sys = [x(1);x(2);x(3);x(4);C*x];



function sys=mdlGetTimeOfNextVarHit(t,x,u)

sampleTime = 1;    %  Example, set the next hit to be one second later.
sys = t + sampleTime;



function sys=mdlTerminate(t,x,u)

sys = [];

% end mdlTerminate
