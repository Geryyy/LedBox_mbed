function [ rx ] = lora2483_radio_tx( s, tx )

tx = sprintf('radio tx %s',dec2hex(tx));
fprintf(s,tx);

while(s.BytesAvailable==0);pause(0.1);end

rx = char(fread(s,s.BytesAvailable)');


   fprintf('%s - %s\n', tx, rx(1:end-2));
   
   while(s.BytesAvailable==0);pause(0.1);end
rx = char(fread(s,s.BytesAvailable)');
   fprintf('TX: %s\n',  rx(1:end-2));



end



