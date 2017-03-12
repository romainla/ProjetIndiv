clear;
configcmu

nameJointAddNoise = 'foot';
noiseRelativeDuration = 0.1; %1 means all the record will have added noise,
                             %0 means no noise will be added, 0.5 means
                             %half of the recording will have added noise..
varNoise = 60; % variance of the Gaussian Noise which will be added
nbMaxEpochs = 200;

button = questdlg(strcat('The current body part on which noise is added is: "',nameJointAddNoise, '" do you want to change it ?'),'Change noisy body part','Yes','No','No');
switch button
    case 'Yes',
        prompt = {'Enter the name of the body part (ensure it exists in the bvh file, case insensitive)'};
        dlg_title = 'Input';
        num_lines = 1;
        defaultans = {nameJointAddNoise};
        answer = inputdlg(prompt,dlg_title,num_lines,defaultans);
        nameJointAddNoise = answer{1};
    case 'No',
end


outputFolder = strcat('./PredictedBVH_',nameJointAddNoise);
activity='walk';
train_prec=0.8;%this is the percentage of training files in terms of all files associated with activity


[datalist_numerics, datalist_strings] = xlsread(datalistfile);
activityindices = find(strcmp(datalist_strings(:,2),activity));
bvhfiles = datalist_strings(:,1);
for i=1:length(activityindices)
    selectedbvhfiles{i} = strcat(bvhfiles{activityindices(i)},'.bvh');
end
%select train files and test files randomly
randomorderedbvhfiles= selectedbvhfiles(randperm(length(selectedbvhfiles)));
train_filenum= length(randomorderedbvhfiles)*train_prec;
train_bvhfiles= randomorderedbvhfiles(1:train_filenum);
test_bvhfiles= randomorderedbvhfiles(train_filenum+1:length(randomorderedbvhfiles));

skeleton = cell(1,length(train_bvhfiles));
skeleton_train_noisy = cell(1,length(train_bvhfiles));
%training stage
for i=1:length(train_bvhfiles)
    bvhfile= train_bvhfiles{i};
    [skeleton{i}]=openBVH(bvhfile);%load bvh file and get all joints information and their corresponding timestamp
    
    %Todo:
    %Step1=> downsampling from 120Hz to 30Hz since Kinect data is 30Hz and we would like to simluate the data as Kinect SDK obtained
    skeleton{i}.data = skeleton{i}.data(:,1:4:end);
    skeleton{i}.descriptions.nbFrames = size(skeleton{i}.data,2);
    %Step2=> add noise to get corrupted joint information
    [skeleton_train_noisy{i}] = addNoiseSelectedJoint(skeleton{i},nameJointAddNoise,noiseRelativeDuration, varNoise);
    
end

%testing stage
skeleton_test = cell(1,length(test_bvhfiles));
skeleton_noisy = cell(1,length(test_bvhfiles));
error_noisy = cell(1,length(test_bvhfiles));

if ~exist(outputFolder,'dir')
    mkdir(outputFolder)
end

for i=1:length(test_bvhfiles)
    bvhfile= test_bvhfiles{i};
    [skeleton_test{i}]=openBVH(bvhfile);%load bvh file and get all joints information and their corresponding timestamp
    
    %Step1=> downsampling from 120Hz to 30Hz since Kinect data is 30Hz and we would like to simluate the data as Kinect SDK obtained
    skeleton_test{i}.data = skeleton_test{i}.data(:,1:4:end);
    skeleton_test{i}.descriptions.nbFrames = size(skeleton_test{i}.data,2);
    %Step2=> add noise to get corrupted joint information
    [skeleton_noisy{i}] = addNoiseSelectedJoint(skeleton_test{i},nameJointAddNoise,noiseRelativeDuration, varNoise);
    
    bvhfile= strcat(outputFolder,'/noisy_',skeleton_noisy{i}.descriptions.nameFiles);
    saveBVH(skeleton_noisy{i},bvhfile);
end

% Prepare data in a dormat which can be used by the framework of G.CHEN
[ X, noiseX, testX, testnoiseX, Y, testY, imh, imw, lambda, dataid, batchsize ] = convertToFrameworkFormat( skeleton, skeleton_train_noisy, skeleton_test,skeleton_noisy );
% To test more quickly the framework, save the data into a workspace 
nameWorkspace = './testerSDAE.mat';
save(nameWorkspace,'X', 'noiseX', 'testX', 'testnoiseX', 'Y', 'testY', 'imh', 'imw', 'lambda', 'dataid', 'batchsize', 'skeleton', 'skeleton_train_noisy', 'skeleton_test', 'skeleton_noisy','outputFolder')
% Train the SDAE
jointlearn_dae_4layers(X, noiseX, Y, batchsize, lambda, dataid);
% Use the SDAE to correct the noise of the skeleto_noisy data
[ dataout ] = predictionFramework( dataid,testX, testnoiseX, batchsize, testY );
% Save the corrected data into skeleton structure to register it into a BVH
% file
[ skeleton_predicted] = convertFromFrameworkToSkeleton( skeleton_test, dataout );
% Save the predicted BVH files
for i=1:length(skeleton_predicted)
    bvhfile= strcat(outputFolder,'/pred_',skeleton_predicted{i}.descriptions.nameFiles);
    saveBVH(skeleton_predicted{i},bvhfile);
end