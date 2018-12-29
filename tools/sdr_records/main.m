
close all;
clear all;
clc;

filename = '29-Dec-2018 134952.478 433.000MHz 000.wav';
Fs = 2.5e6;
[Y,Fs] = audioread(filename,[1,5*Fs]);

t = (1:length(Y))/Fs;

plot(t,Y);