if exist('s')   % Close serial port if already open
    fclose(s);
    delete(s);
    clear s;
end

% Open serial port

% Clear old serial connections (simplifies usage with Matlab)
newobjs = instrfind;
if isempty(newobjs) == 0
    fclose(newobjs);
    delete(newobjs);
    clear newobjs    
end

 % Open serial port
s = serial('COM5');    % Please enter your serial port name here
set(s,'BaudRate',57600,'Terminator','CR/LF');
fopen(s);

if(s.BytesAvailable>0)  % Flush UART-RX buffer
    fread(s,s.BytesAvailable);
end

%%
fprintf('\n --- LoRa2483 Initialization ---------------------------------\n');
lora2483_tx_rx(s,'sys reset');

% communicate with Microchip RN2483 Modul 868 MHz
% lora2483_tx_rx(s,'radio set mod lora');
% lora2483_tx_rx(s,'radio set freq 868100000');
% lora2483_tx_rx(s,'radio set pwr 14');
% lora2483_tx_rx(s,'radio set sf sf12');
% lora2483_tx_rx(s,'radio set afcbw 125');
% lora2483_tx_rx(s,'radio set rxbw 250');
% lora2483_tx_rx(s,'radio set fdev 5000');
% lora2483_tx_rx(s,'radio set prlen 8');
% lora2483_tx_rx(s,'radio set crc on');
% lora2483_tx_rx(s,'radio set cr 4/8');
% lora2483_tx_rx(s,'radio set wdt 5500');
% lora2483_tx_rx(s,'radio set sync 12');
% lora2483_tx_rx(s,'radio set bw 250');
% lora2483_tx_rx(s,'sys get hweui');
% lora2483_tx_rx(s,'mac pause');

% Communicate with Hoby RF Modul 433.1 MHz
lora2483_tx_rx(s,'radio set mod lora');
lora2483_tx_rx(s,'radio set freq 433100000');
lora2483_tx_rx(s,'radio set pwr 14');
lora2483_tx_rx(s,'radio set sf sf10');
lora2483_tx_rx(s,'radio set afcbw 125');
lora2483_tx_rx(s,'radio set rxbw 250');
lora2483_tx_rx(s,'radio set fdev 5000');
lora2483_tx_rx(s,'radio set prlen 6');
lora2483_tx_rx(s,'radio set crc on');
lora2483_tx_rx(s,'radio set cr 4/6');
lora2483_tx_rx(s,'radio set wdt 5500');
lora2483_tx_rx(s,'radio set sync 55');
lora2483_tx_rx(s,'radio set bw 500');
lora2483_tx_rx(s,'sys get hweui');
lora2483_tx_rx(s,'mac pause');

fprintf('\n --- LoRa2483 initialization end ------------------------------\n');
