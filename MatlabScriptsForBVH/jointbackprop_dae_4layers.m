% 
%
% Code provided Gang Chen
%
% 

maxepoch=200;
fprintf(1,'\nFine-tuning deep autoencoder by minimizing cross entropy error. \n');
fprintf(1,'60 batches of 1000 cases each. \n');

load([dataid 'vh']);
load([dataid 'hp']);
load([dataid 'hp2']);
load([dataid 'po']); 

[batchdata, noisebatchdata, batchtargets] = genbatch(X, noiseX, batchsize, Y);
[numcases numdims numbatches]=size(batchdata);
N=numcases; 
[~, numclass, ~] = size(batchtargets);
%%%% PREINITIALIZE WEIGHTS OF THE AUTOENCODER %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
w1=[vishid; hidrecbiases];
w2=[hidpen; penrecbiases];
w3=[hidpen2; penrecbiases2];
w4=[hidtop; toprecbiases];
w5=[hidtop'; topgenbiases]; 
w6=[hidpen2'; hidgenbiases2]; 
w7=[hidpen'; hidgenbiases]; 
w8=[vishid'; visbiases];


w9=[hidtop'; topgenbiases]; 
w10=[hidpen2'; hidgenbiases2]; 
w11=[hidpen'; hidgenbiases]; 
w_class=0.1*randn(size(w8,1),numclass);

%%%%%%%%%% END OF PREINITIALIZATIO OF WEIGHTS  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

l1=size(w1,1)-1;
l2=size(w2,1)-1;
l3=size(w3,1)-1;
l4=size(w4,1)-1;
l5=size(w5,1)-1;
l6=size(w6,1)-1;
l7=size(w7,1)-1;
l8=size(w8,1)-1;
l9=l1; 
l10 = numclass;
train_err=[];
train_crerr=[];

for epoch = 1:maxepoch

%%%%%%%%%%%%%%%%%%%% COMPUTE TRAINING RECONSTRUCTION ERROR %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
err=0; 
err_cr =0;
counter =0;
[numcases numdims numbatches]=size(batchdata);
N=numcases;
 for batch = 1:numbatches
  data = [batchdata(:,:,batch)];
  data = [data ones(N,1)];
  
  noisedata = [noisebatchdata(:,:,batch)];
  noisedata = [noisedata ones(N,1)];
  
  target = [batchtargets(:,:,batch)];
  
  w1probs = 1./(1 + exp(-noisedata*w1)); w1probs = [w1probs  ones(N,1)];
  w2probs = 1./(1 + exp(-w1probs*w2)); w2probs = [w2probs ones(N,1)];
  w3probs = 1./(1 + exp(-w2probs*w3)); w3probs = [w3probs  ones(N,1)];
  w4probs = w3probs*w4; w4probs = [w4probs  ones(N,1)];
  % reconstruction
  w5probs = 1./(1 + exp(-w4probs*w5)); w5probs = [w5probs  ones(N,1)];
  w6probs = 1./(1 + exp(-w5probs*w6)); w6probs = [w6probs  ones(N,1)];
  w7probs = 1./(1 + exp(-w6probs*w7)); w7probs = [w7probs  ones(N,1)];
  dataout = 1./(1 + exp(-w7probs*w8));
  err= err +  1/N*sum(sum( (data(:,1:end-1)-dataout).^2 )); 
  % classification
 w9probs = 1./(1 + exp(-w4probs*w9)); w9probs = [w9probs  ones(N,1)];
 w10probs = 1./(1 + exp(-w9probs*w10)); w10probs = [w10probs ones(N,1)];
 w11probs = 1./(1 + exp(-w10probs*w11)); w11probs = [w11probs  ones(N,1)];
 targetout = exp(w11probs*w_class);
 targetout = targetout./repmat(sum(targetout,2),1,numclass);

 [I J]=max(targetout,[],2);
 [I1 J1]=max(target,[],2);
 counter=counter+length(find(J==J1));
 err_cr = err_cr- sum(sum( target(:,1:end).*log(targetout))) ;
  
  end
 train_err(epoch)=1- counter/(numbatches*batchsize);
 train_crerr(epoch)=err_cr/numbatches;
 
 counter
%%%%%%%%%%%%%% END OF COMPUTING TRAINING RECONSTRUCTION ERROR %%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% %%%% DISPLAY FIGURE TOP ROW REAL DATA BOTTOM ROW RECONSTRUCTIONS %%%%%%%%%%%%%%%%%%%%%%%%%
% fprintf(1,'Displaying in figure 1: Top row - real data, Bottom row -- reconstructions \n');
% output=[];
%  for ii=1:15
%   output = [output data(ii,1:end-1)' dataout(ii,:)'];
%  end
%    if epoch==1 
%    close all 
%    figure('Position',[100,600,1000,200]);
%    else 
%    figure(1)
%    end 
%    mnistdisp(output);
%    drawnow;
% 


 tt=0;
 for batch = 1:numbatches/10
 fprintf(1,'epoch %d batch %d\r',epoch,batch);

%%%%%%%%%%% COMBINE 10 MINIBATCHES INTO 1 LARGER MINIBATCH %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 tt=tt+1; 
 data=[];
 noisedata= [];
 targets =[];
 for kk=1:10
 data=[data 
       batchdata(:,:,(tt-1)*10+kk)]; 
   noisedata=[noisedata 
     noisebatchdata(:,:,(tt-1)*10+kk)];
 targets=[targets
       batchtargets(:,:,(tt-1)*10+kk)];
 end 

%%%%%%%%%%%%%%% PERFORM CONJUGATE GRADIENT WITH 3 LINESEARCHES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  max_iter=3;
  VV = [w1(:)' w2(:)' w3(:)' w4(:)' w5(:)' w6(:)' w7(:)' w8(:)' w9(:)' w10(:)' w11(:)' w_class(:)']';
  Dim = [l1; l2; l3; l4; l5; l6; l7; l8; l9; numclass];

  % [X, fX] = minimize(VV,'CG_MNIST',max_iter,Dim,data);
  [X, fX] = minimize(VV,'JOINT_BACK_CG_4LAYER',max_iter,Dim, data, noisedata, targets, lambda);

  w1 = reshape(X(1:(l1+1)*l2),l1+1,l2);
  xxx = (l1+1)*l2;
  w2 = reshape(X(xxx+1:xxx+(l2+1)*l3),l2+1,l3);
  xxx = xxx+(l2+1)*l3;
  w3 = reshape(X(xxx+1:xxx+(l3+1)*l4),l3+1,l4);
  xxx = xxx+(l3+1)*l4;
  w4 = reshape(X(xxx+1:xxx+(l4+1)*l5),l4+1,l5);
  xxx = xxx+(l4+1)*l5;
  w5 = reshape(X(xxx+1:xxx+(l5+1)*l6),l5+1,l6);
  xxx = xxx+(l5+1)*l6;
  w6 = reshape(X(xxx+1:xxx+(l6+1)*l7),l6+1,l7);
  xxx = xxx+(l6+1)*l7;
  w7 = reshape(X(xxx+1:xxx+(l7+1)*l8),l7+1,l8);
  xxx = xxx+(l7+1)*l8;
  w8 = reshape(X(xxx+1:xxx+(l8+1)*l9),l8+1,l9);
  xxx = xxx + (l8+1)*l9;
  w9 = reshape(X(xxx+1:xxx+(l5+1)*l6),l5+1,l6);
  xxx = xxx+(l5+1)*l6;
  w10 = reshape(X(xxx+1:xxx+(l6+1)*l7),l6+1,l7);
  xxx = xxx+(l6+1)*l7;
  w11 = reshape(X(xxx+1:xxx+(l7+1)*l8),l7+1,l8);
  xxx = xxx+(l7+1)*l8;
  w_class = reshape(X(xxx+1:xxx+(l8+1)*numclass),l8+1,numclass);

 end
%%%%%%%%%%%%%%% END OF CONJUGATE GRADIENT WITH 3 LINESEARCHES %%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
end
%%%%%%%%%%%%%%% END OF LEARNING %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if(exist(dataid,'var')) || ~isempty(dataid) 
     save([dataid '_weights.mat'], 'w1', 'w2', 'w3', 'w4', 'w5', 'w6', 'w7', 'w8', 'w9', 'w10', 'w11', 'w_class');
     save([dataid '_error.mat'],  'train_err');
     save([dataid '_data.mat'], 'dataout');
 else
    save mnist_weights w1 w2 w3 w4 w5 w6 w7 w8 w9 w10 w11 w_class
    save mnist_error train_err;
    save mnist_dataout dataout;
 end
