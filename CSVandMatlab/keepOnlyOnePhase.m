function [ objectSelectedPhase ] = keepOnlyOnePhase( object, label )
%keepOnlyOnePhase Function creating an object with the data corresponding
%to the selected phase of exercise
%   This function creates an object containing the data of object belonging
%   to the selected phase of exercise. It also creates attributes
%   corresponding to each column of the Data attribute and named by the
%   corresponding NameColumns value

objectSelectedPhase = object;
objectSelectedPhase.Data = object.Data(strcmp(object.Data(:,2),label)==1,:);

for j=1:object.NbColumns
    if(~strcmp(objectSelectedPhase.NameColumns{j},'PhaseOfExercise'))
        % Conversion of the cells into array
        objectSelectedPhase.(objectSelectedPhase.NameColumns{j}) = str2double(objectSelectedPhase.Data(:,j));
    end
end
objectSelectedPhase.NbFrames = size(objectSelectedPhase.Data,1);
end

