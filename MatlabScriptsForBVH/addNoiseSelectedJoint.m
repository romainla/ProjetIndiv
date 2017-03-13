function [ skeleton_noisy ] = addNoiseSelectedJoint( skeleton, labelJoint, relativeDuration, varNoise )
%addNoiseSelectedJoint Create a copy of the skeleton, adding Gaussian Noise
%on the selected joint for the indicated relative duration
%   Detailed explanation goes here
muNoise = 0;

skeleton_noisy = skeleton;
nbFrameOfNoise = floor(relativeDuration * skeleton.descriptions.nbFrames);
correspondingIndexes = [];
if ~strcmp(labelJoint,'all')
for i=1:skeleton.descriptions.nbChannels
   if ~isempty(regexpi(skeleton.descriptions.nameDegrees{i} ,labelJoint))
       correspondingIndexes = [correspondingIndexes i];
   end
end
else
   correspondingIndexes = 1:1:skeleton.descriptions.nbChannels;
end
noise = sqrt(varNoise).*randn(length(correspondingIndexes),nbFrameOfNoise) + muNoise;
start = floor(rand(1,1)*(skeleton.descriptions.nbFrames - nbFrameOfNoise));
if start<1
   start = 1; 
end
skeleton_noisy.data(correspondingIndexes,start:start + nbFrameOfNoise - 1)= skeleton.data(correspondingIndexes,start:start + nbFrameOfNoise-1) + noise;

end

