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
    
%     % start infinite receiving "radio rx 0"
%     fprintf(s,'radio rx 0');
%     % wait for response
%     while(s.BytesAvailable==0);pause(0.5);end
%     % read ack response
%     rx = char(fread(s,s.BytesAvailable)');
%     fprintf('%s - %s\n', 'radio rx 0', rx(1:end-2));
% 
%     % wait for actual data
%     while(s.BytesAvailable==0);pause(0.5);end
%     % read data
%     rx = char(fread(s,s.BytesAvailable)');
%     
%     % extract actual message from rn2483 reponse "radio_rx <rx_msg>" 
%     splitstr = strsplit(rx,' ');
%     
%     if(length(splitstr)>1)
%         rx_msg = splitstr{2};
%     else
%         disp(rx);
%         continue;
%     end
%     
%     % ascii -> int array
%     rx_data = (sscanf(rx_msg,"%2x"));      
%     % remove SMP frame
%     smpReceiveBytes(rx_data,instance); %Send the received bytes to the smp library
% 
%     % check for received frames in smp library
%     if smpMessagesToReceive(instance) == 0 %Check if the smp library has received a valid message
%         error('Data should be correct and therefore the receive counter should be greater than zero\n');
%     end
%     [receivedMessage_raw, success] = smpGetNextReceiveMessage(instance); %Get received message from the buffer
% 
%     
%     % convert received integer values to hex string
%     disp('received Message');
%     disp(receivedMessage_raw);
%     receivedMessage = dec2hex(receivedMessage_raw);
%     if(length(receivedMessage)>0)
%         rxmsg =  strjoin(string(receivedMessage));
%         rxmsg = stripblanks(rxmsg);
%         % display
%         fprintf('Received: \n%s\n', rxmsg);
%     else
%         fprintf('no message received\n');
%         continue;
%     end
%     
%     pause(0.5);
%     
%     % generate smp frame
%     smpSendBytes(receivedMessage_raw,instance); %Prepare message for sending
%     if smpSendMessagesCount(instance) == 0 %Check how many messages are in the send buffer
%         error('smp should have one message in the buffer at this point');
%     end
%     [sendMessage, success] = smpGetNextSendMessage(instance); %Get one message to send over the interface
%     if ~success
%         error('smp send Error');
%     end
%    
%     txmsg = dec2hex(sendMessage);
%     txmsg = strjoin(string(txmsg));
%     txmsg = stripblanks(txmsg);
%     
    pause(1);
    txmsg = 'deadbeef';
    % send somethin back 
    fprintf('\nsend return message: \n');
    fprintf(s,'radio tx %s\n',txmsg);
    % wait for response
    while(s.BytesAvailable==0);pause(0.5);end
    % read ack response
    rx = char(fread(s,s.BytesAvailable)');
    disp(rx);
    
%     while(s.BytesAvailable==0);pause(0.5);end
%     % read ack response
%     rx = char(fread(s,s.BytesAvailable)');
%     disp(rx);
    pause(0.1);
    
end

closeRadio;