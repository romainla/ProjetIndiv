function jointlearn_dae_4layers(X, noiseX, batchsize, lambda, dataid)


% 
%
% Code provided by Gang Chen  
% gangchen@buffalo.edu
%
% close all

maxepoch=50; %In the Science paper we use maxepoch=50, but it works just fine. 


% for mnist 
% numhid=100; % numpen=64; numpen2=250; 
numhid =400; numpen=200;numpen2=250; 
%for mnist
% numopen=64;
numopen = 100;

[batchdata, noisebatchdata] = genbatch(X, noiseX, batchsize);

[numcases numdims numbatches]=size(batchdata);

%% the first layer is rbm
fprintf(1,'Pretraining Layer 1 with RBM: %d-%d \n',numdims,numhid);
restart=1;
rbm; %grbm
hidrecbiases=hidbiases; 
save([dataid 'vh.mat'], 'vishid', 'hidrecbiases', 'visbiases');

%% the second layer is hid rbm 
fprintf(1,'\nPretraining Layer 2 with RBM: %d-%d \n',numhid,numpen);
batchdata=batchposhidprobs;
numhid=numpen;
restart=1;
rbm;
hidpen=vishid; penrecbiases=hidbiases; hidgenbiases=visbiases;
save([dataid 'hp.mat'], 'hidpen', 'penrecbiases', 'hidgenbiases');


%% the third layer is hid rbm 

fprintf(1,'\nPretraining Layer 3 with RBM: %d-%d \n',numpen,numpen2);
batchdata=batchposhidprobs;
numhid=numpen2;
restart=1;
rbm;
hidpen2=vishid; penrecbiases2=hidbiases; hidgenbiases2=visbiases;
save([dataid 'hp2.mat'], 'hidpen2', 'penrecbiases2', 'hidgenbiases2');

% fprintf(1,'\nPretraining Layer 4 with RBM: %d-%d \n',numpen2,numopen);
fprintf(1,'\nPretraining Layer 4 with RBM: %d-%d \n',numhid,numopen);
batchdata=batchposhidprobs;
numhid=numopen; 
restart=1;
rbmhidlinear;
hidtop=vishid; toprecbiases=hidbiases; topgenbiases=visbiases;
save([dataid 'po.mat'], 'hidtop', 'toprecbiases', 'topgenbiases');

jointbackprop_dae_4layers; 
