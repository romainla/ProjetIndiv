function [ skeleton_noisy ] = addNoiseSelectedJoint( skeleton, labelJoint, relativeDuration )
%addNoiseSelectedJoint Create a copy of the skeleton, adding Gaussian Noise
%on the selected joint for the indicated relative duration
%   Detailed explanation goes here
muNoise = 0;
varNoise = 60;

skeleton_noisy = skeleton;
nbFrameOfNoise = floor(relativeDuration * skeleton.descriptions.nbFrames);
correspondingIndexes = [];
for i=1:skeleton.descriptions.nbChannels
   if ~isempty(regexpi(skeleton.descriptions.nameDegrees{i} ,labelJoint))
       correspondingIndexes = [correspondingIndexes i];
   end
end
noise = sqrt(varNoise).*randn(length(correspondingIndexes),nbFrameOfNoise) + muNoise;
start = floor(rand(1,1)*(skeleton.descriptions.nbFrames - nbFrameOfNoise));
skeleton.data(correspondingIndexes,start:start + nbFrameOfNoise - 1)= skeleton.data(correspondingIndexes,start:start + nbFrameOfNoise-1) + noise;

end

