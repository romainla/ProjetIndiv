clear;
configcmu

nameJointAddNoise = 'foot';

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
train_prec=0.5;%this is the percentage of training files in terms of all files associated with activity


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
%training stage
for i=1:length(train_bvhfiles)
    bvhfile= train_bvhfiles{i};
    [skeleton{i}]=openBVH(bvhfile);%load bvh file and get all joints information and their corresponding timestamp
    
    %Todo:
    %Step1=> downsampling from 120Hz to 30Hz since Kinect data is 30Hz and we would like to simluate the data as Kinect SDK obtained
    skeleton{i}.data = skeleton{i}.data(:,1:4:end);
    skeleton{i}.descriptions.nbFrames = size(skeleton{i}.data,2);
    %Step2=> add noise to get corrupted joint information
    
    %Step3=> preparing the data to train an autoencoder nerual network
    if i==1
        trainingSet = zeros(size(skeleton{i}.data,1),0);
    end
    trainingSet  = [trainingSet skeleton{i}.data]; 
end
%Step3=> training a autoencoder nerual network to reconstruct the pose
autoenc_rxyz = trainAutoencoder(trainingSet);


%testing stage
skeleton_test = cell(1,length(test_bvhfiles));
skeleton_noisy = cell(1,length(test_bvhfiles));
skeleton_predicted = cell(1,length(test_bvhfiles));
error = cell(1,length(test_bvhfiles));
for i=1:length(test_bvhfiles)
    bvhfile= test_bvhfiles{i};
    [skeleton_test{i}]=openBVH(bvhfile);%load bvh file and get all joints information and their corresponding timestamp
    
    %Todo:
    %Step1=> downsampling from 120Hz to 30Hz since Kinect data is 30Hz and we would like to simluate the data as Kinect SDK obtained
    skeleton_test{i}.data = skeleton_test{i}.data(:,1:4:end);
    skeleton_test{i}.descriptions.nbFrames = size(skeleton_test{i}.data,2);
    %Step2=> add noise to get corrupted joint information
    [skeleton_noisy{i}] = addNoiseSelectedJoint(skeleton_test{i},nameJointAddNoise,0.1);
    
    %Step3=> using the autoencoder nerual network trained during training stage
    testSet  = skeleton_noisy{i}.data; 
    resultPredict = predict(autoenc_rxyz, testSet);
    skeleton_predicted{i} = skeleton_test{i};
    skeleton_predicted{i}.data = resultPredict;
    error{i} = abs(resultPredict - testSet);
end

if ~exist(outputFolder,'dir')
    mkdir(outputFolder)
end

for i=1:length(skeleton_predicted)
    bvhfile= strcat(outputFolder,'/',skeleton_predicted{i}.descriptions.nameFiles);
    saveBVH(skeleton_predicted{i},bvhfile);
end