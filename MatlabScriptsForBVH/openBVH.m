function [skeleton] = openBVH(refFileName)
%openBVH Open all the files in the designated folders and create the
%corresponding list of objects containing the data
%   Detailed explanation goes here

skeleton.descriptions.nameFiles = refFileName;
skeleton.descriptions.nameDegrees = cell(1,1);

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
                skeleton.descriptions.nameDegrees{index,1} = strcat(nameJoint, '-',nameDegrees{j+2});
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
    skeleton.descriptions.nbFrames = nbFrames;
    skeleton.descriptions.nbChannels = nbChannels;
    raw = fgetl(refFile); %raw = Frame Time
    index = 0;
    C_data1 = [];
    while index < nbFrames
        data = textscan(refFile,[repmat('%f',[1,nbChannels])],'CollectOutput',1);
        C_data1 = [C_data1;data{1}];
        index = size(C_data1,1);
    end
    skeleton.data = C_data1';
    fclose(refFile);

end

