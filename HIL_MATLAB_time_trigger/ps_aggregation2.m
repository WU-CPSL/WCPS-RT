function [u,trigger,seq_no] = ps_aggregation2 ()
global delta_n delta_r socket_counter2 ul triggerl seq_nol
u=ul;
trigger=triggerl;
seq_no = seq_nol;
if rem(socket_counter2,delta_n/delta_r)==0
    t = tcpip('172.20.166.72', 8000);
    fopen(t);
   
    fwrite(t,"1")
    data=strcat(fread(t, 27, 'uint8')');
    
    results = str2num(data)
    if size(results,1)~=0
        u = results(2);
        trigger = results(3);
        seq_no = results(1);
    end
    ul = u;
    triggerl = trigger;
    seq_nol = seq_no;
    fclose(t);
end
socket_counter2=socket_counter2+1;