function [ X, noiseX, testX, testnoiseX, imh, imw, lambda, dataid, batchsize ] = convertToFrameworkFormat( skeleton, skeleton_noisy, skeleton_test,skeleton_test_noisy )
%convertToFrameworkFormat Convert the data to be able to use the framework developed by G.CHEN
%	Inputs:
%
%	Outputs:
%	-X: matrice of size nbLines*nbColumns where nbLines = nb of %		files and nbColumns = size of a file. It contains the %		noisy-free data used for the learning phase
%	-noiseX: matrice of size nbLines*nbColumns where nbLines = 
% 		nb of files and nbColumns = size of a file. It 	
%		contains the noisy data used for the learning phase
%	-testX: matrice of size nbLines*nbColumns where nbLines = nb
%		of files and nbColumns = size of a file. It contains 
%		the noisy-free data used for testing the network
%	-testnoiseX: matrice of size nbLines*nbColumns where nbLines
% 		=nb of files and nbColumns = size of a file. It 	
%		contains the noisy data used for testing the network
%	-imh: height of the "images". Each column will correspond to
%		one sample of motion capture so imh = nbofchannels-3
%		(We consider that the X, Y and Z translation of the 
%		root are not noisy so don't need to go through the
%		network)
%	-imw: width of the "images". As every "images" must have the %		same size, the shorter motion capture data will be 
%		padded with 0s
%	-lambda: parameter for the Stacked Deep AutoEncoder
%	-dataid: name for registering the final data
%	-batchsize: number of files which form a group for training

batchsize = 16; 
dataid = 'motionCapture'; 
lambda = 1; 

imh = skeleton{1}.descriptions.nbChannels - 6;
numberTrainingFiles = size(skeleton,2);
numberTestFiles = size(skeleton_test,2);

%First, look for the longest motion capture data
longestSample = -1;
for i=1:numberTrainingFiles
	if longestSample < skeleton{i}.descriptions.nbFrames
		longestSample = skeleton{i}.descriptions.nbFrames;
	end
end
for i=1:numberTestFiles 
	if longestSample < skeleton_test{i}.descriptions.nbFrames
		longestSample = skeleton_test{i}.descriptions.nbFrames;
	end
end

imw = longestSample;

% Then, register data in the corresponding matrices, normalizing
% them in the range [0; 1]
X = zeros(numberTrainingFiles ,imh * imw);
noiseX = zeros(numberTrainingFiles ,imh * imw);

testX = zeros(numberTestFiles ,imh * imw);
testnoiseX = zeros(numberTestFiles ,imh * imw);

for i=1:numberTrainingFiles
	temp = skeleton{i}.data(7:end,:);
	temp = temp(:); %becomes a vector where each raw of the
				% skeleton data is written successively
	temp = temp./360 + 1/2; %normalize data between 0 and 1
	X(i,1:skeleton{i}.descriptions.nbFrames * imh) = temp;

	temp = skeleton_noisy{i}.data(7:end,:);
	temp = temp(:); %becomes a vector where each raw of the
				% skeleton data is written successively
	temp = temp./360 + 1/2; %normalize data between 0 and 1
	noiseX(i,1:skeleton_noisy{i}.descriptions.nbFrames * imh) = temp;

end

for i=1:numberTestFiles 
	temp = skeleton_test{i}.data(7:end,:);
	temp = temp(:); %becomes a vector where each raw of the
				% skeleton data is written successively
	temp = temp./360 + 1/2; %normalize data between 0 and 1
	testX(i,1:skeleton_test{i}.descriptions.nbFrames * imh) = temp;

	temp = skeleton_test_noisy{i}.data(7:end,:);
	temp = temp(:); %becomes a vector where each raw of the
				% skeleton data is written successively
	temp = temp./360 + 1/2; %normalize data between 0 and 1
	testnoiseX(i,1:skeleton_test_noisy{i}.descriptions.nbFrames * imh) = temp;

end

end


