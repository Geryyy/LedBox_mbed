function [str] = bytes2String(input)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
start_index = 11; % ignore "radio rx " pattern
str =  char(sscanf(input(start_index:end), '%2x').');
end

