clear all; close all;
%% General parameters
trainingFolderBVH = './trainingSetBVHfiles';
testFolderBVH = './testSetBVHfiles';
resultsFolderBVH = './resultBVHfiles';

%% Opening the files
button = questdlg(strcat('The current name of the training set folder is: "',trainingFolderBVH, '" do you want to change it ?'),'Change training set folder','Yes','No','No');
switch button
    case 'Yes',
        prompt = {'Enter training set folder:'};
        dlg_title = 'Input';
        num_lines = 1;
        defaultans = {trainingFolderBVH};
        answer = inputdlg(prompt,dlg_title,num_lines,defaultans);
        trainingFolderBVH = answer{1};
    case 'No',
end


button = questdlg(strcat('The current name of the test set folder is: "',testFolderBVH, '" do you want to change it ?'),'Change training set folder','Yes','No','No');
switch button
    case 'Yes',
        prompt = {'Enter test set folder:'};
        dlg_title = 'Input';
        num_lines = 1;
        defaultans = {testFolderBVH};
        answer = inputdlg(prompt,dlg_title,num_lines,defaultans);
        testFolderBVH = answer{1};
    case 'No',
end


button = questdlg(strcat('The current name of the results folder is: "',resultsFolderBVH, '" do you want to change it ?'),'Change training set folder','Yes','No','No');
switch button
    case 'Yes',
        prompt = {'Enter results folder:'};
        dlg_title = 'Input';
        num_lines = 1;
        defaultans = {resultsFolderBVH};
        answer = inputdlg(prompt,dlg_title,num_lines,defaultans);
        resultsFolderBVH = answer{1};
    case 'No',
end

[listTrainingObjects] = openBVH(trainingFolderBVH);
[listTestObjects] = openBVH(testFolderBVH);

%% Check validity of the sets
[listTrainingObjects, listBadTrainingBVH] = checkValiditySet(listTrainingObjects);
[listTestObjects, listTestTrainingBVH] = checkValiditySet(listTestObjects);