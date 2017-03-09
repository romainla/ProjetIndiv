function [refApproxi, refDetails, exApproxi, exDetails] = scriptWavelet(ref_object, ex_object, nameAttribut)
%% Discrete wavelets analysis
lev   = floor(log2(ref_object.FrameRate/2)); 
wname = 'db2';

[C, L] = wavedec(ref_object.(nameAttribut),lev,wname);
refApproxi = appcoef(C,L,wname);
refDetails = detcoef(C,L,'cells');

[C2, L2] = wavedec(ex_object.(nameAttribut),lev,wname);
exApproxi = appcoef(C2,L2,wname);
exDetails = detcoef(C2,L2,'cells');

%% Reconstruction with only approximation coefficients
onlyApproxi = zeros(size(C));
onlyApproxi(1:L(1)) = refApproxi;
onlyApproxi(L(1)+1:L(1)+L(2)) = refDetails{3};
onlyApproxi(L(2)+1:L(2)+L(3)) = refDetails{2};
ref_reconstructed_signal = waverec(onlyApproxi, L,wname);

onlyApproxi2 = zeros(size(C2));
onlyApproxi2(1:L2(1)) = exApproxi;
onlyApproxi2(L2(1)+1:L2(1)+L2(2)) = exDetails{3};
onlyApproxi2(L2(2)+1:L2(2)+L2(3)) = exDetails{2};
ex_reconstructed_signal = waverec(onlyApproxi2, L2,wname);

figure;
subplot(2,1,1);plot(ref_object.TimeStamp,ref_object.(nameAttribut),'r',ref_object.TimeStamp,ref_reconstructed_signal,'b');
legend('Reference signal','Reference low frequencies');
subplot(2,1,2);plot(ex_object.TimeStamp,ex_object.(nameAttribut),'r',ex_object.TimeStamp,ex_reconstructed_signal,'b');
legend('Exercise signal','Exercise low frequencies');
title(strcat(strrep(nameAttribut,'_',' '),' for the reference (up) and exercise (down) signals'));
%plotSignal(ref_reconstructed_signal,ref_object.TimeStamp,ex_reconstructed_signal, ex_object.TimeStamp, 'Low frequencies');
%% Computation of the error between the approximation reconstructed signals and the original ones
ref_error = abs(ref_reconstructed_signal - ref_object.(nameAttribut));
ex_error = abs(ex_reconstructed_signal - ex_object.(nameAttribut));
% figure;
% plot(ref_object.TimeStamp,ref_error,'r')
% hold on;
% plot(ex_object.TimeStamp,ex_error,'b');
% legend('Reference error','Exercise error');
plotSignal(ref_error,ref_object.TimeStamp,ex_error, ex_object.TimeStamp, 'Low frequencies error');
clc;
disp(strcat('Norm of ref_error: ',num2str(norm(ref_error))));
disp(strcat('Norm of ex_error: ',num2str(norm(ex_error))));
disp(strcat('Mean of ref_error: ',num2str(mean(ref_error))));
disp(strcat('Mean of ex_error: ',num2str(mean(ex_error))));
disp(strcat('Std of ref_error: ',num2str(std(ref_error))));
disp(strcat('Std of ex_error: ',num2str(std(ex_error))));

%% Reconstruction with only approximation coefficients
onlyDetails =  C - onlyApproxi;
ref_det_reconstructed_signal = waverec(onlyDetails, L,wname);

onlyDetails2 = C2 - onlyApproxi2;
ex_det_reconstructed_signal = waverec(onlyDetails2, L2,wname);

plotSignal(ref_det_reconstructed_signal,ref_object.TimeStamp,ex_det_reconstructed_signal, ex_object.TimeStamp, 'High frequencies');
%% Computation of the error between the approximation reconstructed signals and the original ones
ref_det_error = abs(ref_det_reconstructed_signal);
ex_det_error = abs(ex_det_reconstructed_signal);
plotSignal(ref_det_error,ref_object.TimeStamp,ex_det_error, ex_object.TimeStamp, 'High frequencies error');
disp(strcat('Norm of ref_det_error: ',num2str(norm(ref_det_error))));
disp(strcat('Norm of ex_det_error: ',num2str(norm(ex_det_error))));
disp(strcat('Mean of ref_det_error: ',num2str(mean(ref_det_error))));
disp(strcat('Mean of ex_det_error: ',num2str(mean(ex_det_error))));
disp(strcat('Std of ref_det_error: ',num2str(std(ref_det_error))));
disp(strcat('Std of ex_det_error: ',num2str(std(ex_det_error))));
end

%% Affichage wavelet coefficients
%len = ref_object.NbFrames;
%Fs = ref_object.FrameRate;
%cfd = zeros(lev,len);
%for k = 1:lev
%    d = detcoef(C,L,k);
%    d = d(:)';
%    d = d(ones(1,2^k),:);
%    cfd(k,:) = wkeep1(d(:)',len);
%end
%cfd =  cfd(:);

%I = find(abs(cfd)<sqrt(eps));
%cfd(I) = zeros(size(I));
%cfd = reshape(cfd,lev,len);

% nbColor = 64;
% labs = cell(lev,1);
% cfd = wcodemat(cfd,nbColor,'row');
%       
% figure;
% colormap(pink(nbColor));
% image(cfd);
% tics = 1:lev;
% for k=1:1:lev
%     labs{k} = strcat('[',int2str(Fs/2^(k+1)),'Hz;',int2str(Fs/2^k),'Hz]');
% end
% set(gca,...
%     'YTickLabelMode','manual','YDir','normal', ...
%     'Box','On','YTick',tics,'YTickLabel',labs);
% title(strcat(nameObject,' attribut ', nameAttribut, ' wavelet detail coefficients'));
% xlabel('Number of frame')
% ylabel('Level');