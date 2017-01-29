%% General parameters
refFileName = 'reference.csv';
exerciseFileName = 'whole_right_abs.csv';

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

if exist(refFileName,'file')
    refFile = fopen(refFileName, 'r');
else
    error('Sorry the desired reference file could not be found. Check if the path, name and extension were correct before to relaunch this script');
end

if exist(refFileName,'file')
    exerciseFile = fopen(exerciseFileName, 'r');
else
    error('Sorry the desired exercice file could not be found. Check if the path, name and extension were correct before to relaunch this script');
end

var


