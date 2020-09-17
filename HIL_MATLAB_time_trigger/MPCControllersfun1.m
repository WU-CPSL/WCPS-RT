function [sys,x0,str,ts,simStateCompliance] = MPCControllersfun1(t,x,u,flag,A,B,C,uini,Xsp,delta_t,delta_r,Atrace,increase_threshold,decrease_threshold,test_period,delay_s,delay_KF)

switch flag,

  case 0,
    [sys,x0,str,ts,simStateCompliance]=mdlInitializeSizes(uini);

  case 1,
    sys=mdlDerivatives(t,x,u);

  case 2,
    sys=mdlUpdate(t,x,u,A,B,C,Xsp,delta_t,delta_r,Atrace,increase_threshold,decrease_threshold,test_period,delay_s,delay_KF);

  case 3,
    sys=mdlOutputs(t,x,u);

  case 4,
    sys=mdlGetTimeOfNextVarHit(t,x,u);

  case 9,
    sys=mdlTerminate(t,x,u);

  otherwise
    DAStudio.error('Simulink:blocks:unhandledFlag', num2str(flag));

end


function [sys,x0,str,ts,simStateCompliance]=mdlInitializeSizes(uini)


sizes = simsizes;
sizes.NumContStates  = 0;
sizes.NumDiscStates  = 3;% current pump voltage, stepcounter, ideal pump voltage
sizes.NumOutputs     = 6;   %wired voltage
sizes.NumInputs      = 4;   %x1, x2, x3, x4
sizes.DirFeedthrough = 0;
sizes.NumSampleTimes = 1;   % at least one sample time is needed
sys = simsizes(sizes);


x0  = [uini;1;uini];
global Buff1 Last1
 for k=1:size(Buff1,2);
     Buff1(k)=x0(1);
 end
Last1=x0(1);
 
str = [];
ts  = [0 0];
simStateCompliance = 'UnknownSimState';


function sys=mdlDerivatives(t,x,u)

sys = [];

function sys=mdlUpdate(t,x,u,A,B,C,Xsp,delta_t,delta_r,Atrace,increase_threshold,decrease_threshold,test_period,delay_s,delay_KF)
global Buff1 Last1 TRACEAA1 TRACEA_f1 UU_OUT1
Nstates = 4;
Win=3;
     if (x(2)>1)&&(x(2)<=Win)
    sumA=0;
         for j=1:x(2)-1
             sumA=sumA+TRACEAA1(j);
         end
         TRACEA_f1(x(2))=sumA/(x(2)-1);
     elseif x(2)>Win
    sumA=0;
        for j=x(2)-Win:x(2)-1
            sumA=sumA+TRACEAA1(j);
        end
         TRACEA_f1(x(2))=sumA/(Win);            
     end
if ((delay_s+delay_KF<delta_t)&&(rem(x(2),delta_t/delta_r)==round((delay_s+delay_KF)/delta_r)))||((delay_s+delay_KF==delta_t)&&(rem(x(2),delta_t/delta_r)==0))

x1=u(1);
x2=u(2);
x3=u(3);
x4=u(4);
%


MA=A;
MB=B;
SSMA=delta_t*MA+eye(Nstates);
SSMB=delta_t*MB;

SSMC=eye(Nstates);
Q=eye(4);
K = [-1.9393  -13.1373    0.0842  -13.0264];
MR = 0.05;
P=lyap((A+B*K)',A+B*K,Q);
K=-(MR+SSMB'*P*SSMB)^(-1)*SSMB'*P*SSMA;
uu =  K*[u(1);u(2);u(3);u(4)];
if uu>=200
    uu = 200;
end

if uu<=-200
    uu = -200;
end

object = [x1 x2 x3 x4]*P*[x1 x2 x3 x4]';

global Costfunction1
Costfunction1=[Costfunction1 object];

global triggerS1 triggerT1 triggerTD1 triggertimeI1 triggertimeD1 triggerB1 triggerSS1

global control_cost1 MSobject1

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Algorithm%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if object>=increase_threshold%10000 re_tx increase threshold (500_10)
    if triggertimeI1==0
     if triggerS1==1
        triggerS1=1;
     else
       triggerS1=triggerS1/2;
       triggertimeI1=triggertimeI1+1;
       MSobject1=object;
       triggertimeD1=0;
       triggerT1=round(x(2)/20);
       control_cost1=control_cost1+1;
     end
    elseif triggertimeI1>0&&(round(x(2)/20)-triggerT1>10)
        if object>=MSobject1*exp(-1.22*(10^(-7))*(round(x(2)/20)-triggerT1>10))
            if triggerS1==1
                triggerS1=1;
            else
            triggerS1=triggerS1/2;
            triggertimeI1=triggertimeI1+1;
            MSobject1=object;
            triggertimeD1=0;
            triggerT1=round(x(2)/20);
            control_cost1=control_cost1+1;
            end
        end
    end
end


if object<=decrease_threshold
    triggerB1(round(x(2)/20)+1)=0;
else 
    triggerB1(round(x(2)/20)+1)=1;
end

sum=1;
if round(x(2)/20)>test_period
    sum=0;
    for k=(round(x(2)/20)-test_period):round(x(2)/20)
        sum=sum+triggerB1(k);
    end
end

    if sum==0
          
              if TRACEA_f1(x(2))>=0.7
               if triggertimeD1==0
                    if triggerS1==4
                        triggerS1=4;

                    else
                        triggerS1=triggerS1*2;
                        triggerTD1=round(x(2)/20);
                        triggertimeD1=triggertimeD1+1;
                        triggertimeI1=0;
                        control_cost1=control_cost1+1;
                    end
               else
                   if round(x(2)/20)-triggerTD1>test_period%400
                       if triggerS1==4
                            triggerS1=4;
                        else
                            triggerS1=triggerS1*2;
                            triggerTD1=round(x(2)/20);
                            triggertimeD1=triggertimeD1+1;
                            triggertimeI1=0;
                            control_cost1=control_cost1+1;
                       end 
                   end
               end
             end
    end

    if t>200
        triggerS1=1;
    end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
triggerS1;
triggerSS1=[triggerSS1 triggerS1];

 
if Atrace(x(2))==1
     for k=1:size(Buff1,2)
        Buff1(k)=0;
     end
 else
     for k=1:(size(Buff1,2)-1)
        Buff1(k)=0;
     end                  
end
Buff1(1)=uu;
UU_OUT1=[UU_OUT1;Buff1];
end                              
 x(1) = Buff1(1);
Last1=Buff1(size(Buff1,2));
 x(2) = x(2) + 1;
 x(3) = Buff1(1);
sys = [x];

%=============================================================================
% mdlOutputs
% Return the block outputs.
%=============================================================================
%
function sys=mdlOutputs(t,x,u)
global Buff1;
global triggerS1;
sys = [Buff1 triggerS1];



function sys=mdlGetTimeOfNextVarHit(t,x,u)

sampleTime = 1;    %  Example, set the next hit to be one second later.
sys = t + sampleTime;



function sys=mdlTerminate(t,x,u)

sys = [];
% end mdlTerminate
