function [result] = ps_aggregation (ymea,mode)
global delta_n delta_r socket_counter
result = 0;
if rem(socket_counter,delta_n/delta_r)==0
    result = SocketHandle(ymea,mode);
    socket_counter;
end
socket_counter=socket_counter+1;