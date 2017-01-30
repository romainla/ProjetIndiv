function [] = plotSignal( reference,referenceTime, exercise, exerciseTime, label )
    %plotSignal Function creating a plot showing the two signals over time
%   This function creates plot subdivided in two subplots: one for the
%   reference and one for the exercise.
    
    figure
    label = strrep(label,'_',' ');
    titleFig = strcat(label,': evolution over time for reference (up) and exercise (down)');
    subplot(2,1,1); plot(referenceTime, reference, 'b')
    xlabel(label);
    subplot(2,1,2); plot(exerciseTime, exercise, 'r')
    xlabel(label);
    title(titleFig);
end