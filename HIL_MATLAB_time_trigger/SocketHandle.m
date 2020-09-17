%%Matlab function for handling socket communications between Matlab and
% Python event server

function result=SocketHandle(ymea,mode)
    t = tcpip('172.20.166.70', 10000);
    fopen(t);
    if abs(ymea)<0.01
        ymea=0;
    end
    message =  sprintf('%08.3g%08g',ymea,mode);
    result=message
    fwrite(t,message);    
    fclose(t);    
end