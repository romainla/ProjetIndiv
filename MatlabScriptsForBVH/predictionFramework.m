function [ dataout ] = predictionFramework( dataid,testX, testnoiseX, batchsize )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
%
%
% Code provided Gang Chen
%
%
%load([dataid 'vh']);
%load([dataid 'hp']);
%load([dataid 'hp2']);
%load([dataid 'po']);

[testbatchdata,testnoisebatchdata] = genbatch(testX, testnoiseX, batchsize);
[numcases numdims numbatches]=size(testnoisebatchdata);
N=numcases;
%%%% PREINITIALIZE WEIGHTS OF THE AUTOENCODER %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%w1=[vishid; hidrecbiases];
%w2=[hidpen; penrecbiases];
%w3=[hidpen2; penrecbiases2];
%w4=[hidtop; toprecbiases];
%w5=[hidtop'; topgenbiases];
%w6=[hidpen2'; hidgenbiases2];
%w7=[hidpen'; hidgenbiases];
%w8=[vishid'; visbiases];
load([dataid '_weights.mat'], 'w1', 'w2', 'w3', 'w4', 'w5', 'w6', 'w7', 'w8');

data = testnoisebatchdata(:,:);
data = [data ones(N,1)];
w1probs = 1./(1 + exp(-data*w1)); w1probs = [w1probs  ones(N,1)];
w2probs = 1./(1 + exp(-w1probs*w2)); w2probs = [w2probs ones(N,1)];
w3probs = 1./(1 + exp(-w2probs*w3)); w3probs = [w3probs  ones(N,1)];
w4probs = w3probs*w4; w4probs = [w4probs  ones(N,1)];
w5probs = 1./(1 + exp(-w4probs*w5)); w5probs = [w5probs  ones(N,1)];
w6probs = 1./(1 + exp(-w5probs*w6)); w6probs = [w6probs  ones(N,1)];
w7probs = 1./(1 + exp(-w6probs*w7)); w7probs = [w7probs  ones(N,1)];
dataout = 1./(1 + exp(-w7probs*w8));


end

