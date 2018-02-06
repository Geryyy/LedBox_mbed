function [ rx ] = lora2483_radio_rx( s )

fprintf(s,'radio rx 0');

while(s.BytesAvailable==0);pause(0.1);end

rx = char(fread(s,s.BytesAvailable)');

fprintf('%s - %s\n', 'radio rx 0', rx(1:end-2));

while(s.BytesAvailable==0);pause(0.1);end

rx = char(fread(s,s.BytesAvailable)');
fprintf('Received: %s\n', rx(1:end-2));

end



