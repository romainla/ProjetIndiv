function [batchdata, noisebatchdata] = genbatch(X, noiseX, batchsize)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

% for training
[N, numdims] = size(X);
N = floor(N/batchsize)*batchsize;

%Create batches
numbatches= floor(N/batchsize);
batchdata = zeros(batchsize, numdims, numbatches);
noisebatchdata = zeros(batchsize, numdims, numbatches);
groups= repmat(1:numbatches, 1, batchsize);
groups= groups(1:N);
groups = groups(randperm(N));
for i=1:numbatches
    batchdata(:,:,i)= X(groups==i,:);
    noisebatchdata(:,:,i)= noiseX(groups==i,:);
end



end

