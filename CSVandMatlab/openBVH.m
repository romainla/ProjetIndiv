function [listObjects] = openBVH(folderBVH)
%openBVH Open all the files in the designated folders and create the
%corresponding list of objects containing the data
%   Detailed explanation goes here

if ~exist(folderBVH,'dir')
    error(strcat('Sorry the desired folder',folderBVH ,'could not be found.'));
end

listingTrainingSet = dir(strcat(folderBVH,'/*.bvh'));
nbTrainingFile = size(listingTrainingSet,1);

listObjects.descriptions.nameFiles = cell(nbTrainingFile,1);
listObjects.descriptions.nameFolder=strcat(folderBVH,'/');
listObjects.descriptions.nameDegrees = cell(nbTrainingFile,1);

listObjects.data = cell(nbTrainingFile,1);
for i=1:nbTrainingFile
    refFileName = listingTrainingSet(i).name;
    listObjects.descriptions.nameFiles{i} = refFileName;
    refFileName = strcat(listObjects.descriptions.nameFolder,refFileName);
    refFile = fopen(refFileName, 'r');
    
    %Takes the root information
    raw = fgetl(refFile); %raw = HIERARCHY
    index = 1;
    while ~strcmp(raw,'MOTION')
        raw = fgetl(refFile); %raw = JOINT xxx or ROOT xxx
        if isempty(strfind(raw, 'End Site')) && isempty(strfind(raw, '}')) && isempty(strfind(raw, 'MOTION'))
            nameJoint = textscan(raw,'%s','Delimiter',' ','MultipleDelimsAsOne',1);
            nameJoint = nameJoint{1}{2,1};
            raw = fgetl(refFile); %raw = {
            raw = fgetl(refFile); %raw = OFFSET
            raw = fgetl(refFile); %raw = CHANNELS
            nameDegrees  = textscan(raw,'%s','Delimiter',' ','MultipleDelimsAsOne',1);
            nameDegrees = nameDegrees{1};
            for j=1:length(nameDegrees)-2
                listObjects.descriptions.nameDegrees{i}{index,1} = strcat(nameJoint, '-',nameDegrees{j+2});
                index = index + 1;
            end
        else    if ~isempty(strfind(raw, 'End Site'))
                    raw = fgetl(refFile); %raw = {
                    raw = fgetl(refFile); %raw = OFFSET
                    raw = fgetl(refFile); %raw = }
                end
        end
    end
    nbChannels = index - 1;
    nbFrames = fscanf(refFile, 'Frames:\t%d\n',[1 1]);
    listObjects.descriptions.nbFrames(i,1) = nbFrames;
    listObjects.descriptions.nbChannels(i,1) = nbChannels;
    raw = fgetl(refFile); %raw = Frame Time
    C_data1 = textscan(refFile,[repmat('%f',[1,nbChannels])],'CollectOutput',1);
    listObjects.data{i} = C_data1{1}';
    fclose(refFile);
end

end

