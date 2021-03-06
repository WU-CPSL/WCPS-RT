function [sys,x0,str,ts,simStateCompliance] = EKFsfun(t,x,u,flag,A,B,C,delta_t,delta_r,delay_s)

switch flag,

  case 0,
    [sys,x0,str,ts,simStateCompliance]=mdlInitializeSizes();

  case 1,
    sys=mdlDerivatives();

  case 2,
    sys=mdlUpdate(t,x,u,A,B,C,delta_t,delta_r,delay_s);

  case 3,
    sys=mdlOutputs(t,x,u,delta_t,delta_r);

  case 4,
    sys=mdlGetTimeOfNextVarHit(t,x,u);

  case 9,
    sys=mdlTerminate(t,x,u);

  otherwise
    DAStudio.error('Simulink:blocks:unhandledFlag', num2str(flag));

end


function [sys,x0,str,ts,simStateCompliance]=mdlInitializeSizes()


sizes = simsizes;
sizes.NumContStates  = 0;
sizes.NumDiscStates  = 5;   % x1, x2,x3, x4, count
sizes.NumOutputs     = 4;   % x1, x2,x3, x4,
sizes.NumInputs      = 3;   % x1,trace, u
sizes.DirFeedthrough = 0;
sizes.NumSampleTimes = 1;   % at least one sample time is needed
sys = simsizes(sizes);
x0=[0; 3; 2; 0;1];
str = [];
ts  = [0 0];
simStateCompliance = 'UnknownSimState';


function sys=mdlDerivatives()

sys = [];

function sys=mdlUpdate(t,x,u,A,B,C,delta_t,delta_r,delay_s)


Q=0.5*eye(4);%predict accuracy
R=1;

global jX bX bP jP Kk uekf xekf
jX=[x(1);x(2);x(3);x(4)];
if ((delay_s<delta_t)&&(rem(x(5),delta_t/delta_r)==round(delay_s/delta_r)))||((delay_s==delta_t)&&(rem(x(5),delta_t/delta_r)==0))%0
x_M=u(1);%x2 measurement
TraceS=u(2);
U=u(3);
uekf=[uekf;x(5),u(3)];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
MA=A;
MB=B;
MC=C;
SSMA=delta_t*MA+eye(4);
SSMB=delta_t*MB;

bX=SSMA*jX+SSMB*U;
AJ=SSMA;
bP=AJ*jP*AJ'+Q;
Kk=bP*MC'*(inv(MC*bP*MC'+R));
jX=bX+Kk*(x_M-MC*bX)*TraceS;  
jP=bP-Kk*MC*bP;
end
xekf=[xekf; x'];
x(1)=jX(1);
x(2)=jX(2);
x(3)=jX(3);
x(4)=jX(4);
x(5)=x(5)+1;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
sys = x;


function sys=mdlOutputs(t,x,u,delta_t,delta_r)

sys = [x(1);x(2);x(3);x(4)];

function sys=mdlGetTimeOfNextVarHit(t,x,u)

sampleTime = 1;    %  Example, set the next hit to be one second later.
sys = t + sampleTime;

function sys=mdlTerminate(t,x,u)

sys = [];
% end mdlTerminate
