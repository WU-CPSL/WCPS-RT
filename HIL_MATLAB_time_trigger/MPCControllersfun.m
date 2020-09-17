function [sys,x0,str,ts,simStateCompliance] = MPCControllersfun(t,x,u,flag,A,B,C,uini,Xsp,delta_t,delta_r,Atrace,increase_threshold,decrease_threshold,test_period,delay_s,delay_KF)

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
sizes.NumDiscStates  = 3;   
sizes.NumOutputs     = 6;  
sizes.NumInputs      = 4;   %x1, x2, x3, x4
sizes.DirFeedthrough = 0;
sizes.NumSampleTimes = 1;   % at least one sample time is needed
sys = simsizes(sizes);


x0  = [uini;1;uini];
global Buff Last
 for k=1:size(Buff,2);
     Buff(k)=x0(1);
 end
Last=x0(1);
 
str = [];
ts  = [0 0];
simStateCompliance = 'UnknownSimState';


function sys=mdlDerivatives(t,x,u)

sys = [];

function sys=mdlUpdate(t,x,u,A,B,C,Xsp,delta_t,delta_r,Atrace,increase_threshold,decrease_threshold,test_period,delay_s,delay_KF)
global Buff Last TRACEAA TRACEA_f UU_OUT
Nstates = 4;
Win=3;
     if (x(2)>1)&&(x(2)<=Win)
    sumA=0;
         for j=1:x(2)-1
             sumA=sumA+TRACEAA(j);
         end
         TRACEA_f(x(2))=sumA/(x(2)-1);
     elseif x(2)>Win
    sumA=0;
        for j=x(2)-Win:x(2)-1
            sumA=sumA+TRACEAA(j);
        end
         TRACEA_f(x(2))=sumA/(Win);            
     end
if ((delay_s+delay_KF<delta_t)&&(rem(x(2),delta_t/delta_r)==round((delay_s+delay_KF)/delta_r)))||((delay_s+delay_KF==delta_t)&&(rem(x(2),delta_t/delta_r)==0))

x1=u(1);
x2=u(2);
x3=u(3);
x4=u(4);
Q=eye(4);
%K=-(MR+SSMB'*P*SSMB)^(-1)*SSMB'*P*SSMA;
K = [-1.9393  -13.1373    0.0842  -13.0264];
uu =  K*[u(1);u(2);u(3);u(4)];
if uu>=200
    uu = 200;
end

if uu<=-200
    uu = -200;
end

MA=A;
MB=B;
SSMA=delta_t*MA+eye(Nstates);
SSMB=delta_t*MB;
SSMC=eye(Nstates);
P=lyap((A+B*K)',A+B*K,Q);
object = [x1 x2 x3 x4]*P*[x1 x2 x3 x4]';

global Costfunction
Costfunction=[Costfunction object];

global triggerS triggerT triggerTD triggertimeI triggertimeD triggerB triggerSS

global control_cost MSobject

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Algorithm%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if object>=increase_threshold%10000 re_tx increase threshold (500_10)
    if triggertimeI==0
     if triggerS==1
        triggerS=1;
     else
       triggerS=triggerS/2;
       triggertimeI=triggertimeI+1;
       MSobject=object;
       triggertimeD=0;
       triggerT=round(x(2)/20);
       control_cost=control_cost+1;
     end
    elseif triggertimeI>0&&(round(x(2)/20)-triggerT>10)
        if object>=MSobject*exp(-(1/313.5798)*(round(x(2)/20)-triggerT))
            if triggerS==1
                triggerS=1;
            else
            triggerS=triggerS/2;
            triggertimeI=triggertimeI+1;
            MSobject=object;
            triggertimeD=0;
            triggerT=round(x(2)/20);
            control_cost=control_cost+1;
            end
        end
    end
end


if object<=decrease_threshold%1000 switch back threshold (500 10)
    triggerB(round(x(2)/20)+1)=0;
else 
    triggerB(round(x(2)/20)+1)=1;
end

sum=1;
if round(x(2)/20)>test_period%500 consecutive decrease time
    sum=0;
    for k=(round(x(2)/20)-test_period):round(x(2)/20)% decrease time
        sum=sum+triggerB(k);
    end
end

    if sum==0
          
              if TRACEA_f(x(2))>=0.7
               if triggertimeD==0
                    if triggerS==4
                        triggerS=4;

                    else
                        triggerS=triggerS*2;
                        triggerTD=round(x(2)/20);
                        triggertimeD=triggertimeD+1;
                        triggertimeI=0;
                        control_cost=control_cost+1;
                    end
               else
                   if round(x(2)/20)-triggerTD>test_period%400
                       if triggerS==4
                            triggerS=4;
                        else
                            triggerS=triggerS*2;
                            triggerTD=round(x(2)/20);
                            triggertimeD=triggertimeD+1;
                            triggertimeI=0;
                            control_cost=control_cost+1;
                       end 
                   end
               end
             end
    end
    
    
    if t>200 % reset for next round of simulation
        triggerS=1;
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
triggerS;
triggerSS=[triggerSS triggerS];

 
if Atrace(x(2))==1
     for k=1:size(Buff,2)
        Buff(k)=0;
     end
 else
     for k=1:(size(Buff,2)-1)
        Buff(k)=0;
     end                  
end
Buff(1) = uu;
UU_OUT=[UU_OUT;Buff];
end                              
 x(1) = Buff(1);
Last=Buff(size(Buff,2));
 x(2) = x(2) + 1;
 x(3) = Buff(1);
sys = [x];

%=============================================================================
% mdlOutputs
% Return the block outputs.
%=============================================================================
%
function sys=mdlOutputs(t,x,u)
global Buff;
global triggerS;
sys = [Buff triggerS];



function sys=mdlGetTimeOfNextVarHit(t,x,u)

sampleTime = 1;    %  Example, set the next hit to be one second later.
sys = t + sampleTime;



function sys=mdlTerminate(t,x,u)

sys = [];
% end mdlTerminate
