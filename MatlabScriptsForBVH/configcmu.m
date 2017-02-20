listingBVHsets = dir('./cmuc*');
for i=1:length(listingBVHsets)
    nameFolder =strcat('./',listingBVHsets(i).name) ;
    if exist(nameFolder,'dir')
        listInFolder = dir(strcat(nameFolder,'/*'));
        for j = 1:length(listInFolder)
            nameBis = strcat(nameFolder,'/',listInFolder(j).name);
            if exist(nameBis,'dir') && isempty(strfind(nameBis(3:end),'.'))
                addpath(genpath(nameBis));
            end
        end
    end
end
%addpath(genpath('./cmuconvert-mb2-01-09/02'));
%addpath(genpath('./cmuconvert-mb2-01-09/03'));
%addpath(genpath('./cmuconvert-mb2-01-09/04'));
%addpath(genpath('./cmuconvert-mb2-01-09/05'));
%addpath(genpath('./cmuconvert-mb2-01-09/06'));
%addpath(genpath('./cmuconvert-mb2-01-09/07'));
%addpath(genpath('./cmuconvert-mb2-01-09/08'));
%addpath(genpath('./cmuconvert-mb2-01-09/09'));

datalistfile='./cmuconvert-mb2-01-09/cmu-mocap-index-spreadsheet.xls';