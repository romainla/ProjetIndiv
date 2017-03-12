% 
%
% Code provided by Gang Chen

function [f, df] = BACK_CG_4LAYER(VV,Dim,XX, noiseXX, targets, lambda);

l1 = Dim(1);
l2 = Dim(2);
l3 = Dim(3);
l4= Dim(4);
l5= Dim(5);
l6= Dim(6);
l7= Dim(7);
l8= Dim(8);
l9= Dim(9);
numclass = Dim(10);
N = size(XX,1);

% Do decomversion.
 w1 = reshape(VV(1:(l1+1)*l2),l1+1,l2);
 xxx = (l1+1)*l2;
 w2 = reshape(VV(xxx+1:xxx+(l2+1)*l3),l2+1,l3);
 xxx = xxx+(l2+1)*l3;
 w3 = reshape(VV(xxx+1:xxx+(l3+1)*l4),l3+1,l4);
 xxx = xxx+(l3+1)*l4;
 w4 = reshape(VV(xxx+1:xxx+(l4+1)*l5),l4+1,l5);
 xxx = xxx+(l4+1)*l5;
 w5 = reshape(VV(xxx+1:xxx+(l5+1)*l6),l5+1,l6);
 xxx = xxx+(l5+1)*l6;
 w6 = reshape(VV(xxx+1:xxx+(l6+1)*l7),l6+1,l7);
 xxx = xxx+(l6+1)*l7;
 w7 = reshape(VV(xxx+1:xxx+(l7+1)*l8),l7+1,l8);
 xxx = xxx+(l7+1)*l8;
 w8 = reshape(VV(xxx+1:xxx+(l8+1)*l9),l8+1,l9);
  xxx = xxx + (l8+1)*l9;
  w9 = reshape(VV(xxx+1:xxx+(l5+1)*l6),l5+1,l6);
  xxx = xxx+(l5+1)*l6;
  w10 = reshape(VV(xxx+1:xxx+(l6+1)*l7),l6+1,l7);
  xxx = xxx+(l6+1)*l7;
  w11 = reshape(VV(xxx+1:xxx+(l7+1)*l8),l7+1,l8);
  xxx = xxx+(l7+1)*l8;
  w_class = reshape(VV(xxx+1:xxx+(l8+1)*numclass),l8+1,numclass);
  
  

  XX = [XX ones(N,1)];
  noiseXX = [noiseXX ones(N,1)];
  w1probs = 1./(1 + exp(-noiseXX*w1)); w1probs = [w1probs  ones(N,1)];
  w2probs = 1./(1 + exp(-w1probs*w2)); w2probs = [w2probs ones(N,1)];
  w3probs = 1./(1 + exp(-w2probs*w3)); w3probs = [w3probs  ones(N,1)];
  w4probs = w3probs*w4; w4probs = [w4probs  ones(N,1)];
  w5probs = 1./(1 + exp(-w4probs*w5)); w5probs = [w5probs  ones(N,1)];
  w6probs = 1./(1 + exp(-w5probs*w6)); w6probs = [w6probs  ones(N,1)];
  w7probs = 1./(1 + exp(-w6probs*w7)); w7probs = [w7probs  ones(N,1)];
  XXout = 1./(1 + exp(-w7probs*w8));
  
  
  % classification
  w9probs = 1./(1 + exp(-w4probs*w9)); w9probs = [w9probs  ones(N,1)];
  w10probs = 1./(1 + exp(-w9probs*w10)); w10probs = [w10probs ones(N,1)];
  w11probs = 1./(1 + exp(-w10probs*w11)); w11probs = [w11probs  ones(N,1)];
  targetout = exp(w11probs*w_class);
  targetout = targetout./repmat(sum(targetout,2),1,numclass);
  
  

f = -1/N*sum(sum( XX(:,1:end-1).*log(XXout) + (1-XX(:,1:end-1)).*log(1-XXout))) + ...
     -1/N*lambda*sum(sum( targets.*log(targetout) + (1-targets).*log(1-targetout)));

% reconstruction 
IO = 1/N*(XXout-XX(:,1:end-1));
Ix8=IO; 
dw8 =  w7probs'*Ix8;

Ix7 = (Ix8*w8').*w7probs.*(1-w7probs); 
Ix7 = Ix7(:,1:end-1);
dw7 =  w6probs'*Ix7;

Ix6 = (Ix7*w7').*w6probs.*(1-w6probs); 
Ix6 = Ix6(:,1:end-1);
dw6 =  w5probs'*Ix6;

Ix5 = (Ix6*w6').*w5probs.*(1-w5probs); 
Ix5 = Ix5(:,1:end-1);
dw5 =  w4probs'*Ix5;

%classification
IO2 = 1/N*lambda*(targetout-targets);
Ix_class=IO2; 
dw_class =  w11probs'*Ix_class;

Ix11 = (Ix_class*w_class').*w11probs.*(1-w11probs); 
Ix11 = Ix11(:,1:end-1);
dw11 =  w10probs'*Ix11;

Ix10 = (Ix11*w11').*w10probs.*(1-w10probs); 
Ix10 = Ix10(:,1:end-1);
dw10 =  w9probs'*Ix10;

Ix9 = (Ix10*w10').*w9probs.*(1-w9probs); 
Ix9 = Ix9(:,1:end-1);
dw9 =  w4probs'*Ix9;


Ix4 = (Ix5*w5')   + lambda*Ix9*w9';
Ix4 = Ix4(:,1:end-1);
dw4 =  w3probs'*Ix4;

Ix3 = (Ix4*w4').*w3probs.*(1-w3probs); 
Ix3 = Ix3(:,1:end-1);
dw3 =  w2probs'*Ix3;

Ix2 = (Ix3*w3').*w2probs.*(1-w2probs); 
Ix2 = Ix2(:,1:end-1);
dw2 =  w1probs'*Ix2;

Ix1 = (Ix2*w2').*w1probs.*(1-w1probs); 
Ix1 = Ix1(:,1:end-1);
%dw1 =  XX'*Ix1;
dw1 =  noiseXX'*Ix1;

df = [dw1(:)' dw2(:)' dw3(:)' dw4(:)' dw5(:)' dw6(:)'  dw7(:)'  dw8(:)' dw9(:)' dw10(:)' dw11(:)' dw_class(:)']'; 