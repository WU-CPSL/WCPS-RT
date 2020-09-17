function [result] = ps_aggregation1 (ymea,mode)
global delta_n delta_r socket_counter1
result = 0;
if rem(socket_counter1,delta_n/delta_r)==0
    result = SocketHandle1(ymea,mode);
    socket_counter1;
end
socket_counter1=socket_counter1+1;