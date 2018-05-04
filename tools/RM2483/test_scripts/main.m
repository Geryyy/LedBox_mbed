% brief: main Skript fuer LEDBox Kommunikationstest mit LORA Modul und SMP
% author: Gerald Ebmer
% date: 28.03.2018

%% header
clear all;
close all;
clc;

addpath('../../libsmp/Matlab/'); % Pfad zu SMP-DLL
addpath('../../libsmp/Matlab/smpDriver/'); % Pfad zu SMP-DLL

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
    rx_msg = splitstr{2};
    
    % remove SMP frame
    smpReceiveBytes(rx_msg,instance); %Send the received bytes to the smp library
    % check for received frames in smp library
    disp(strcat('rx_msg: ', rx_msg));
    if smpMessagesToReceive(instance) == 0 %Check if the smp library has received a valid message
        fprintf('Data should be correct and therefore the receive counter should be greater than zero\n');
    end
    [receivedMessage, success] = smpGetNextReceiveMessage(instance); %Get received message from the buffer
    % convert bytes to string
    if(length(receivedMessage)>0)
        str =  bytes2String(receivedMessage);
        % display
        fprintf('Received: \n%s\n', str);
    else
        fprintf('no message received\n');
    end
    
    %% send somethin back 
    fprintf('\nsend return message: \n');
    fprintf(s,'radio tx 5555');
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