function [u,trigger,seq_no] = ps_aggregation21()
global delta_n delta_r socket_counter21 ul1 triggerl1 seq_nol1 triggerR1
u=ul1;
trigger=triggerl1;
seq_no = seq_nol1;
if rem(socket_counter21,delta_n/delta_r)==0
    t = tcpip('172.20.166.72', 14000);
    fopen(t);
   
    fwrite(t,"1")
    data=strcat(fread(t, 27, 'uint8')');
    
    results = str2num(data)
    if size(results,1)~=0
        u = results(2);
        trigger = results(3);
        seq_no = results(1);
    end
    ul1 = u;
    triggerl1 = trigger;
    triggerR1 = [triggerR1 triggerl1];
    seq_nol1 = seq_no;
    fclose(t);
end
socket_counter21=socket_counter21+1;