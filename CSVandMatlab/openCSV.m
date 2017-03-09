function [ refObject, exerciseObject ] = openCSV( refFileName, exerciseFileName)
%openCSV This function opens the csv files and record the data in objects
%   The two files designed by the input names are opened. Their information
%   are extracted and stored in two objects.

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

refObject.NbFrames = 0;
refObject.NbFrames = fscanf(refFile, 'Frames: %d\n',[1 1]);
refObject.FrameRate = 0.0;
refObject.FrameRate = fscanf(refFile, 'Frame rate: %f\n',[1 1]);

exerciseObject.NbFrames = 0;
exerciseObject.NbFrames = fscanf(exerciseFile, 'Frames: %d\n',[1 1]);
exerciseObject.FrameRate = 0.0;
exerciseObject.FrameRate = fscanf(exerciseFile, 'Frame rate: %f\n', [1 1]);

raw = fgetl(refFile);
[nameColumns]  = textscan(raw,'%s','Delimiter',' ');
refObject.NameColumns = nameColumns{1};
refObject.NbColumns = size(refObject.NameColumns, 1);

raw = fgetl(exerciseFile);
[nameColumns]  = textscan(raw,'%s','Delimiter',' ');
exerciseObject.NameColumns = nameColumns{1};
exerciseObject.NbColumns = size(exerciseObject.NameColumns, 1);

limit = refObject.NbFrames;
nbColumns = refObject.NbColumns;
Data = cell(limit,nbColumns);
for i=1:limit
    raw = fgetl(refFile);
    data = textscan(raw,'%s','Delimiter',' ');
    data = (data{1}');
    for j=1:nbColumns
       refObject.Data{i,j} = data{j};
    end
    %for j=1:nbColumns
    %  Data{i,j} = data{j};
    %end
end

%for j=1:nbColumns
%    refObject.(nameColumns{1}{j}) = Data(:,j);
%end

limit = exerciseObject.NbFrames;
nbColumns = exerciseObject.NbColumns;
exerciseObject.Data = cell(limit,nbColumns);
for i=1:limit
    raw = fgetl(exerciseFile);
    data = textscan(raw,'%s','Delimiter',' ');
    data = (data{1}');
    for j=1:nbColumns
       exerciseObject.Data{i,j} = data{j};
    end
end

fclose(refFile);
fclose(exerciseFile);
end

