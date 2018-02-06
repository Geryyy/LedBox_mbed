function [ rx ] = lora2483_tx_rx( s, tx )


fprintf(s,tx);

while(s.BytesAvailable==0);pause(0.1);end

rx = char(fread(s,s.BytesAvailable)');


   fprintf('%s - %s\n', tx, rx(1:end-2));



end


