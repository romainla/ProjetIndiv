function [] = plotSignal( reference,referenceTime, exercise, exerciseTime, label )
    %plotSignal Function creating a plot showing the two signals over time
%   This function creates plot subdivided in two subplots: one for the
%   reference and one for the exercise.
    minimum = min(min(reference,min(exercise)));
    maximum = max(max(reference, max(exercise)));
    figure
    label = strrep(label,'_',' ');
    titleFig = strcat(label,': evolution over time for reference (up) and exercise (down)');
    subplot(2,1,1); plot(referenceTime, reference, 'b'); ylim([minimum maximum]);
    ylabel(label);
    xlabel('time (s)')
    subplot(2,1,2); plot(exerciseTime, exercise, 'r'); ylim([minimum maximum]);
    xlabel('time (s)')
    ylabel(label);
    title(titleFig);
end