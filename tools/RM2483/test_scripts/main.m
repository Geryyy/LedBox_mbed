% brief: main Skript fuer LEDBox Kommunikationstest mit LORA Modul und SMP
% author: Gerald Ebmer
% date: 28.03.2018

%% header
clear all;
close all;
clc;

% addpath('../../libsmp/Matlab/'); % Pfad zu SMP-DLL
% addpath('../../libsmp/Matlab/smpDriver/'); % Pfad zu SMP-DLL

%% init
instance = smpGetInstance(false); %Use library with Reed Solomon Codes change to false to use the library without reed solomon encoding
initRadio;

%% receive data
fprintf('start receive...\n');
while(1)
    
    % start infinite receiving "radio rx 0"
    fprintf(s,'radio rx 0');
    % wait for response
    while(s.BytesAvailable==0);pause(0.5);end
    % read ack response
    rx = char(fread(s,s.BytesAvailable)');
    fprintf('%s - %s\n', 'radio rx 0', rx(1:end-2));

    % wait for actual data
    while(s.BytesAvailable==0);pause(0.5);end
    % read data
    rx = char(fread(s,s.BytesAvailable)');
    
    % extract actual message from rn2483 reponse "radio_rx <rx_msg>" 
    splitstr = strsplit(rx,' ');
    
    if(length(splitstr)>1)
        rx_msg = splitstr{2};
    else
        disp(rx);
        continue;
    end
    
    % ascii -> int array
    rx_data = (sscanf(rx_msg,"%2x"));      
    % remove SMP frame
    smpReceiveBytes(rx_data,instance); %Send the received bytes to the smp library

    % check for received frames in smp library
%     disp(strcat('rx_msg: ', rx_msg));
    if smpMessagesToReceive(instance) == 0 %Check if the smp library has received a valid message
        fprintf('Data should be correct and therefore the receive counter should be greater than zero\n');
    end
    [receivedMessage, success] = smpGetNextReceiveMessage(instance); %Get received message from the buffer
    % convert bytes to string
    
    disp('received Message');
    disp(receivedMessage);
    receivedMessage = dec2hex(receivedMessage);
    if(length(receivedMessage)>0)
        str =  strjoin(string(receivedMessage));
        str = stripblanks(str);
        % display
        fprintf('Received: \n%s\n', str);
    else
        fprintf('no message received\n');
    end
    
    pause(0.1);
    %% send somethin back 
    fprintf('\nsend return message: \n');
    fprintf(s,'radio tx %s\n',str);
    % wait for response
    while(s.BytesAvailable==0);pause(0.5);end
    % read ack response
    rx = char(fread(s,s.BytesAvailable)');
    disp(rx);
    
%     while(s.BytesAvailable==0);pause(0.5);end
%     % read ack response
%     rx = char(fread(s,s.BytesAvailable)');
%     disp(rx);
    
end

closeRadio;