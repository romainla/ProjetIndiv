playAllNoise = 0;
playDefinedJointNoisy = 1;
%% Case Gaussian noise on every joints
if playAllNoise ~= 0
    % The noise added to BVH data is a Gaussian noise of variance varNoise
    
    noise = rand(size(trainingSet_target))*varNoise;
    
    % Set and initialize RLS adaptive filter parameters and values:
    
    M      = 40;                 % Filter order
    delta  = varNoise;                % Initial input covariance estimate
    P0     = (1/delta)*eye(M,M); % Initial setting for the P matrix
    
    Hs = cell(size(trainingSet_target,1),1);
    Hn = cell(size(trainingSet_target,1),1);
    Hadapt = cell(size(trainingSet_target,1),1);
    for i=1:size(trainingSet_target,1)
        % The noisy-free data
        Hs{i} = dsp.SignalSource(trainingSet_target(i,:)','SamplesPerFrame',30);
        Hn{i} = dsp.SignalSource(noise(i,:)','SamplesPerFrame',30);
        Hadapt{i} = dsp.RLSFilter(M,'InitialInverseCovariance',P0);
    end
    % Running the RLS adaptive filter for 1000 iterations. As the adaptive
    % filter converges, the filtered noise should be completely subtracted from
    % the "signal + noise". Also the error, 'e', should contain only the
    % original signal.
    
    for k = 1:floor(length(trainingSet_target)/30)
        for i=1:size(trainingSet_target,1)
            n = step(Hn{i}); % Noise
            s = step(Hs{i});
            d = n + s;
            [y,e]  = step(Hadapt{i},n,d);
        end
    end
    
    x = zeros(size(skeleton{1}.data));
    y = zeros(size(skeleton{1}.data));
    e = zeros(size(skeleton{1}.data));
    for i=1:size(trainingSet_target,1)
        x(i,:) = skeleton{1}.data(i,:) + noise(i,1:length(skeleton{1}.data(i,:)));
        [y(i,:),e(i,:)] = step(Hadapt{i},noise(1,1:size(skeleton{1}.data,2)),x(i,:));
    end
    errorNoisyOriginal = norm(x-skeleton{1}.data);
    errorDeNoisedOriginal = norm(e-skeleton{1}.data);
end
%% Case noise only on some defined joints
if playDefinedJointNoisy ~= 0
    % The noise added to BVH data is a Gaussian noise of variance varNoise
    
    noise = trainingSet_signalPlusNoise -trainingSet_target;
    
    % Set and initialize RLS adaptive filter parameters and values:
    
    M      = 40;                 % Filter order
    delta  = varNoise;                % Initial input covariance estimate
    P0     = (1/delta)*eye(M,M); % Initial setting for the P matrix
    
    Hs = cell(size(trainingSet_target,1),1);
    Hn = cell(size(trainingSet_target,1),1);
    Hadapt = cell(size(trainingSet_target,1),1);
    for i=1:size(trainingSet_target,1)
        % The noisy-free data
        Hs{i} = dsp.SignalSource(trainingSet_target(i,:)','SamplesPerFrame',30);
        Hn{i} = dsp.SignalSource(noise(i,:)','SamplesPerFrame',30);
        Hadapt{i} = dsp.RLSFilter(M,'InitialInverseCovariance',P0);
    end
    % Running the RLS adaptive filter for 1000 iterations. As the adaptive
    % filter converges, the filtered noise should be completely subtracted from
    % the "signal + noise". Also the error, 'e', should contain only the
    % original signal.
    
    for k = 1:floor(length(trainingSet_target)/30)
        for i=1:size(trainingSet_target,1)
            n = step(Hn{i}); % Noise
            s = step(Hs{i});
            d = n + s;
            [y,e]  = step(Hadapt{i},n,d);
        end
    end
    
    noise = rand(1,length(trainingSet_target));
    
    for j=1:length(skeleton_train_noisy)
        x = zeros(size(skeleton{j}.data));
        y = zeros(size(skeleton{j}.data));
        e = zeros(size(skeleton{j}.data));
        for i=1:size(trainingSet_target,1)
            x(i,:) = skeleton_train_noisy{j}.data(i,:);
            [y(i,:),e(i,:)] = step(Hadapt{i},noise(1,1:size(skeleton{j}.data,2)),x(i,:));
        end
        errorNoisyOriginal(j) = norm(x-skeleton{j}.data);
        errorDeNoisedOriginal(j) = norm(e-skeleton{j}.data);
    end
end

