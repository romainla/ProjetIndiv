function [] = saveBVH(skeleton, refFileName)
%saveBVH Save the designated skeleton into a BVH file
%   Detailed explanation goes here

refFile = fopen(skeleton.descriptions.nameFiles, 'r');
saveFile = fopen(refFileName, 'w');
raw = '';
while isempty(strfind(raw,'MOTION'))
    raw = fgets(refFile); %raw = JOINT xxx or ROOT xxx
    fprintf(saveFile, raw);
end
raw = fgets(refFile); %raw = Frames
index = strfind(raw,':') + 2;
str = int2str(skeleton.descriptions.nbFrames);
if length(str) ~= length(raw(index:end-1))
   str = [' ' str]; 
end
raw(index:end-1) = str;
fprintf(saveFile, raw);
frameTime = 1/30.0;
raw = fgets(refFile); %raw = Frame time
index = strfind(raw,':') + 2;
raw(index:end-1) = num2str(frameTime);
fprintf(saveFile, raw);
data = skeleton.data';
for i=1:skeleton.descriptions.nbFrames
    raw = fgets(refFile);
    format = [repmat('%f ',[1,skeleton.descriptions.nbChannels-1]) '%f'];
    str = sprintf(format,data(i,:));
    str = [str raw(end-1:end)] ;
    fprintf(saveFile, str);
end
fclose(refFile);
fclose(saveFile);
end

