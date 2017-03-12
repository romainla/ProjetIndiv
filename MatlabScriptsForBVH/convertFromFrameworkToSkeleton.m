function [ skeleton_predicted] = convertFromFrameworkToSkeleton( skeleton, data )
%convertFromFrameworkToSkeleton Convert the predicted data
% obtained using the framework developed by G.CHEN in a skeleton % structure to be able to record them in a BVH thereafter
%	Inputs:
%		-skeleton: cells containing the list of original
%				skeleton
%		-data: contains the predicted values
%	Outputs:
%	-skeleton_predicted: contains the same header information
%		than skeleton with the predicted data to store

skeleton_predicted = skeleton;

imh = skeleton{1}.descriptions.nbChannels - 3;
numberTrainingFiles = size(skeleton,2);

for i=1:numberTrainingFiles
	%temp stores the data of the umpth file
	temp = data(i,1:skeleton{i}.descriptions.nbFrames*imh);
	%denormalize data
	temp = (temp - 1/2).*360; 
	%reshape the data into the form of a skeleton.data
	temp = reshape(temp,imh,skeleton{i}.descriptions.nbFrames);
	%stores the translation data which were not predicted
	skeleton_predicted{i}.data(1:3,:) = 	skeleton{i}.data(1:3,:);
	skeleton_predicted{i}.data(4:end,:) = temp;
end

end


