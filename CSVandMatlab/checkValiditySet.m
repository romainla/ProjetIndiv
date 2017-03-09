function [ listObjects, listBadSets ] = checkValiditySet( listObjects )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
minimum = min(listObjects.descriptions.nbChannels);
maximum = max(listObjects.descriptions.nbChannels);

if maximum ~= minimum
    nbFiles = size(listObjects.descriptions.nameFiles,1);
    index = 1;
    for i=1:nbFiles
        if listObjects.descriptions.nbChannels(i)~= minimum
            listBadSets{index,1} = listObjects.descriptions.nameFiles{i};
            index = index + 1;
        end
    end
end

