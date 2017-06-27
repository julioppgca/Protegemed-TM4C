%     
%      Signal Generation Init Script
%
%  Author: Júlio Santos - julio.ppgca@gmail.com
%  Date: 29/12/2016
%  V1.0
%

% Sampling Parameters
Sample_Number = 2048;
Base_Frequency = 60; %Hz
SampleTime=1/(Base_Frequency*Sample_Number);

% PWM Parameters
CPU_Frequency = 90e6; % F28069M run at 90MHz.
PWM_Frequency = 250e3; % 250KHz is good enought, DAC is a LPF (R=1k, C=0.1uF, Fc=1,591KHz) 
PWM_Counter = CPU_Frequency/PWM_Frequency;
PWM_Offset = PWM_Counter/2;

% Wave Generation Parameters
PWM_Max_Voltage = 3.3; %Volts
PWM_Scale = PWM_Max_Voltage/PWM_Counter;

% Harmonic index generation
Max_Harmonic = 11;
h=zeros(1,Max_Harmonic);
for i=1:Max_Harmonic
    h(i) = ceil(Sample_Number/i)-1;
end

% Wave Construction, all of them up to Max_Harmonic
t = 0:Sample_Number; % Time vector
f=zeros(Max_Harmonic,Sample_Number);
Wave=zeros(Max_Harmonic,Sample_Number);
for i=1:Max_Harmonic
   for j=1:Sample_Number
       f(i,j)=t(j)/h(i);
       Wave(i,j)=sin(2*pi*f(i,j));
   end
end

% Amplitude vector
A = zeros(Max_Harmonic);

% % Test only, really a mess... (:P)
% figure(1); hold off;
% for i=1:Max_Harmonic
% plot(Wave(i,:)); grid; hold on;
% plot(Wave(i,:));
% end

%
%        Electrical Outlet 1 Emulation 
%
% DAC1 = Phase Current (DAC1 is PWM7_A)
% DAC2 = Differential Current (DAC2 is PWM7_B)
% DAC3 = Phase Voltage (DAC3 is PWM8_A)
%

% Phase Current wave
PWM7A_Phase_Amplitude_Volts = 1; % Volts, Max = 3.3V
A(1) = PWM7A_Phase_Amplitude_Volts*((PWM_Counter-3)/3.3);
PWM7A_Phase_Wave = ceil((...
                        A(1)*Wave(1,:) + ...
                        A(1)*0.1*Wave(3,:) + ...
                        A(1)*0.03*Wave(9,:) ...
                        ) + PWM_Offset)';
PWM7A_Phase_RMS = rms(PWM7A_Phase_Wave-PWM_Offset)*PWM_Scale; % Test only

% Differential Current Wave
PWM7B_Phase_Amplitude_Volts = 0.8; % Volts, Max = 3.3V
A(2) = PWM7B_Phase_Amplitude_Volts*((PWM_Counter-3)/3.3);
PWM7B_Phase_Wave = ceil((...
                        A(2)*Wave(1,:) + ...
                        A(2)*0.1*Wave(3,:) + ...
                        A(2)*0.03*Wave(9,:) + ...
                        A(2)*0.3*Wave(5,:) ...
                        ) + PWM_Offset)';
PWM7B_Phase_RMS = rms(PWM7B_Phase_Wave-PWM_Offset)*PWM_Scale; % Test only

% Phase Voltage Wave
PWM8A_Phase_Amplitude_Volts = 1.2; % Volts, Max = 3.3V
A(3) = PWM8A_Phase_Amplitude_Volts*((PWM_Counter-3)/3.3);
PWM8A_Phase_Wave = ceil((...
                        A(3)*Wave(1,:) ...
                        ) + PWM_Offset)';
PWM8A_Phase_RMS = rms(PWM8A_Phase_Wave-PWM_Offset)*PWM_Scale; % Test only
