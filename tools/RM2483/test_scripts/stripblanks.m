function [str] = stripblanks(input)

[str, rem] = strtok(input);
while rem ~= ""
   [tmp, rem] = strtok(rem);
   str = str + tmp;
end

end

