clear;
configcmu

nameJointAddNoise = 'foot';
noiseRelativeDuration = 0.1; %1 means all the record will have added noise,
                             %0 means no noise will be added, 0.5 means
                             %half of the recording will have added noise..
varNoise = 60; % variance of the Gaussian Noise which will be added
nbMaxEpochs = 2000;
sparsityProportion = 0.2;

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
autoenc_rxyz = trainAutoencoder(trainingSet,'ShowProgressWindow',false,'MaxEpochs',nbMaxEpochs,'SparsityProportion',sparsityProportion);


%testing stage
skeleton_test = cell(1,length(test_bvhfiles));
skeleton_noisy = cell(1,length(test_bvhfiles));
error_noisy = cell(1,length(test_bvhfiles));
skeleton_predicted_once = cell(1,length(test_bvhfiles));
error_once = cell(1,length(test_bvhfiles));
skeleton_predicted_twice = cell(1,length(test_bvhfiles));
error_twice = cell(1,length(test_bvhfiles));

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
    error_noisy{i} = abs(skeleton_noisy{i}.data - skeleton_test{i}.data); 
    %Step3=> using the autoencoder neural network trained during training stage
    testSet  = skeleton_noisy{i}.data; 
    resultPredict = predict(autoenc_rxyz, testSet);
    skeleton_predicted_once{i} = skeleton_test{i};
    skeleton_predicted_once{i}.data = resultPredict;
    error_once{i} = abs(resultPredict - skeleton_test{i}.data);
    % Using the autoencoder neural network on the predicted value to see if
    % it improves the results
    testSet  = skeleton_predicted_once{i}.data; 
    resultPredict = predict(autoenc_rxyz, testSet);
    skeleton_predicted_twice{i} = skeleton_test{i};
    skeleton_predicted_twice{i}.data = resultPredict;
    error_twice{i} = abs(resultPredict - skeleton_test{i}.data);
    
    % Recording of the resulting skeleton in bvh format for visualization
    bvhfile= strcat(outputFolder,'/once_noise_duration',num2str(noiseRelativeDuration*100),'_var_',num2str(varNoise),'_',skeleton_predicted_once{i}.descriptions.nameFiles);
    saveBVH(skeleton_predicted_once{i},bvhfile);
    bvhfile= strcat(outputFolder,'/twice_noise_duration',num2str(noiseRelativeDuration*100),'_var_',num2str(varNoise),'_',skeleton_predicted_twice{i}.descriptions.nameFiles);
    saveBVH(skeleton_predicted_twice{i},bvhfile);
    bvhfile= strcat(outputFolder,'/noise_duration',num2str(noiseRelativeDuration*100),'_var_',num2str(varNoise),'_',skeleton_noisy{i}.descriptions.nameFiles);
    saveBVH(skeleton_noisy{i},bvhfile);
end

norm_error = zeros(length(skeleton_noisy),3);
for i=1:length(skeleton_noisy)
norm_error(i,1) = norm(error_noisy{i});
norm_error(i,2) = norm(error_once{i});
norm_error(i,3) = norm(error_twice{i});
end
boxplot(norm_error,'Labels',{strcat(num2str(noiseRelativeDuration*100),'%time',num2str(varNoise),' variance noisy data'),'predicted once', 'predicted twice'})
xlabel('Type of data')
ylabel('Norm of the error')
ylim([0 1000])
title('Norm  of the error between original data and the others')
saveas(gcf,strcat('Epochs',int2str(nbMaxEpochs),'Sparsity',num2str(sparsityProportion*100),'percentNormOfTheErrorDuration',num2str(noiseRelativeDuration*100),'percentVariance',num2str(varNoise),'.png'));