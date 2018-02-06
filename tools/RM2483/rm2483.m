if exist('s')   % Close serial port if already open
    fclose(s);
    delete(s);
    clear s;
end

clear all;
close all;
clc;

% Open serial port

% Clear old serial connections (simplifies usage with Matlab)
newobjs = instrfind;
if isempty(newobjs) == 0
    fclose(newobjs);
    delete(newobjs);
    clear newobjs    
end

 % Open serial port
s = serial('COM8');    % Please enter your serial port name here
set(s,'BaudRate',57600,'Terminator','CR/LF');
fopen(s);

if(s.BytesAvailable>0)  % Flush UART-RX buffer
    fread(s,s.BytesAvailable);
end

%%
fprintf('\n --- LoRa2483 Initialization ---------------------------------\n');
lora2483_tx_rx(s,'sys reset');

lora2483_tx_rx(s,'radio set mod lora');

lora2483_tx_rx(s,'radio set freq 868100000');

lora2483_tx_rx(s,'radio set pwr 14');

lora2483_tx_rx(s,'radio set sf sf12');

lora2483_tx_rx(s,'radio set afcbw 125');

lora2483_tx_rx(s,'radio set rxbw 250');

lora2483_tx_rx(s,'radio set fdev 5000');

lora2483_tx_rx(s,'radio set prlen 8');

lora2483_tx_rx(s,'radio set crc on');

lora2483_tx_rx(s,'radio set cr 4/8');

lora2483_tx_rx(s,'radio set wdt 5500');

lora2483_tx_rx(s,'radio set sync 12');

lora2483_tx_rx(s,'radio set bw 250');

lora2483_tx_rx(s,'sys get hweui');

lora2483_tx_rx(s,'mac pause');

fprintf('\n --- LoRa2483 initialization end ------------------------------\n');

%% Autosend
for ii = 1:1
    lora2483_radio_tx(s,'abc');
    pause(5);
end
%%
lora2483_radio_rx(s);

%%
rec = '78782441475353442c3f2c3f2a31300a';
       
tx = char(sscanf(rec, '%2x').')
txh = sprintf('%2x',tx);

%%lora2483_radio_tx(s,tx);


%%
fclose(s);
clear s;



