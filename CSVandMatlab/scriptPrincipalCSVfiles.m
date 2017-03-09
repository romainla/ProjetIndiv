clear all; close all;
%% General parameters
refFileName = 'reference.csv';
exerciseFileName = 'drinking_unhealthy.csv';

%% Opening the files
button = questdlg(strcat('The current name of the reference file is: "',refFileName, '" do you want to change it ?'),'Change reference file','Yes','No','No');
switch button
    case 'Yes',
        prompt = {'Enter reference file name, with path and .csv extension:'};
        dlg_title = 'Input';
        num_lines = 1;
        defaultans = {refFileName};
        answer = inputdlg(prompt,dlg_title,num_lines,defaultans);
        refFileName = answer{1};
    case 'No',
end


button = questdlg(strcat('The current name of the exercise file is: "',exerciseFileName, '" do you want to change it ?'),'Change reference file','Yes','No','No');
switch button
    case 'Yes',
        prompt = {'Enter exercise file name, with path and .csv extension:'};
        dlg_title = 'Input';
        num_lines = 1;
        defaultans = {exerciseFileName};
        answer = inputdlg(prompt,dlg_title,num_lines,defaultans);
        exerciseFileName = answer{1};
    case 'No',
end

[refFile, exerciseFile] = openCSV(refFileName, exerciseFileName);

%% Creating objects containing the data of only one phase of exercise each
[refInit] = keepOnlyOnePhase(refFile, 'init');
[exerciseInit] = keepOnlyOnePhase(exerciseFile, 'init');

[refExercise] = keepOnlyOnePhase(refFile, 'exercise');
[exerciseExercise] = keepOnlyOnePhase(exerciseFile, 'exercise');

%% Plots of the reference signal and the exercise signal over time for different types of signal
plotSignal(refInit.(refInit.NameColumns{3}),refInit.TimeStamp, exerciseInit.(exerciseInit.NameColumns{3}),exerciseInit.TimeStamp,refInit.NameColumns{3});

plotSignal(refExercise.(refExercise.NameColumns{3}),refExercise.TimeStamp, exerciseExercise.(exerciseExercise.NameColumns{3}),exerciseExercise.TimeStamp,exerciseExercise.NameColumns{3});

%[approxiExercise, detailExercise, approxiRef, detailRef] = scriptWavelet(refExercise,exerciseExercise, exerciseExercise.NameColumns{3});
%Previous line is commented because the wavelet analysis is not needed any
%more for this individual project
