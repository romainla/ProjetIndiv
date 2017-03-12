function [batchdata, noisebatchdata, batchtargets] = genbatch(X, noiseX, batchsize, targets)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

% for training
[N, numdims] = size(X);
N = floor(N/batchsize)*batchsize;

if nargin <4
    nclasses = 10;
    targets= zeros(N, nclasses);
else
    nclasses = size(targets,2);
end

%Create batches
numbatches= floor(N/batchsize);
batchdata = zeros(batchsize, numdims, numbatches);
noisebatchdata = zeros(batchsize, numdims, numbatches);
batchtargets = zeros(batchsize, nclasses, numbatches);
groups= repmat(1:numbatches, 1, batchsize);
groups= groups(1:N);
groups = groups(randperm(N));
for i=1:numbatches
    batchdata(:,:,i)= X(groups==i,:);
    noisebatchdata(:,:,i)= noiseX(groups==i,:);
    batchtargets(:,:,i)= targets(groups==i,:);
end



end

