graphics_toolkit('gnuplot');
pkg load signal
pkg load control

function [h,p,r] = hist2(y1,y2,x); 
% function [h,p,r] = hist2(y1,y2,x); 
%
% y1 and y2 are column vectors with data (must have same dimension)
% x  are centers of histogram bins for the 2 coordinates. 
%    should be equally spaced !!!
% h is the 2-D histogram (y1 distrib in cols, y2 in rows)
% 
% p is the estimate of 2-D prob. dens. function
%
% r is he autocorrelation coefficient computed using the theoretical 
% formula (5-8 in Sebesta) 
%
% ... histogram computation is not very optimized ... 

L = length(x); 
N = length(y1); 

% alloc for hist 
h = zeros(L,L); out = 0; % counter of bad samples...

% make a BIG matrix with all values of x ... will actually do something 
% like vector quantization. 
% first make col vector out of x, then repeat it N times. 
xcol = x(:); bigx = repmat(xcol,1,N); 

% make rows out of y1 , y2 and do the 'quantization' 
yr = y1(:)'; bigy = repmat(yr,L,1);
[dummy,ind1] = min(abs(bigy - bigx)); 
yr = y2(:)'; bigy = repmat(yr,L,1);
[dummy,ind2] = min(abs(bigy - bigx)); 

% now just go through the indices and increment respective cases in h
for ii=1:N,
  d1 = ind1(ii);   d2 = ind2(ii); 
  h(d1,d2) = h(d1,d2) + 1; 
end

%%% prob dens: will have to normalize
surf = (x(2) - x(1))^2; % surface of one tile
p = h / N / surf;  

%%%% autocor coeff 
% make col vector out of x and clone it L times. 
x = x(:); X1 = repmat(x,1,L);
% make row vector out of x and clone it L times. 
x = x'; X2 = repmat(x,L,1); 
% now put it together, don't forget to multipl by tile surface
r = sum(sum (X1 .* X2 .* p)) * surf;

%%% check ... 
check = sum(sum (p)) * surf; 
disp(['hist2: check -- 2d integral should be 1 and is ' num2str(check)]); 
endfunction;

% Author : Matúš Škuta < xskuta04@stud.fit.vutbr.cz >

% 1) Nacitajte signal a napiste jeho vzorkovaciu frekvenciu + jeho dlzku vo vzorkoch a sekundach

fprintf('1)\n');
% Nacitame signal vzorkovacia_frekvencia = pocet vzorkov v 1 sekunde
[signal,vzorkovacia_frekvencia] = audioread('xskuta04.wav');
% Vytvorime si riadkovy vektor
signal = signal';
% Zistime dlzku signalu
s_length = length(signal);

% Zistime si dlzku vzorkov pri 20ms signalu
cut_20ms = int64(vzorkovacia_frekvencia/1000*20);

fprintf('Vzorkovaca frekvencia signálu: %f [Hz].\n',vzorkovacia_frekvencia);
fprintf('Dĺžka signálu vo vzorkoch: %f.\n',s_length);
fprintf('Dĺžka signálu v sekundách: %f [s].\n',s_length / vzorkovacia_frekvencia);
fprintf('Počet binárnych symbolov signálu: %d.\n',s_length/16);

fprintf('----------------------------------------------------------------------------------------------------------\n');
% -----------------------------------------------------------------------------------------------------------------------

fprintf('2)\n');
% Vytvorime si premenne pre i a signal_data
i_2 = 0;
signal_data_2 = [];

% Prejdeme cely signal
while i_2 < length(signal)
  % Bereme vzdy z kazdych 16 prvok 8 prvok
  i_2 = i_2 + 8;
  % Ak je mensi nez 0 tak je to 0
  if signal(i_2) < 0
    signal_data_2 = [signal_data_2,0];
  % AK je vacsi nez 0 alebo rovny 0 tak je 1
  else
    signal_data_2 = [signal_data_2,1];
  endif
  % Pripocitame zvysnych 8
  i_2 = i_2 + 8;
endwhile

s_2 = load('xskuta04.txt');
s_2 = s_2';


counter_2 = 0;
for j_2 = 1:length(s_2)
    if s_2(j_2) == signal_data_2(j_2)
      counter_2 = counter_2 + 1;      
    endif
endfor

if counter_2 == length(s_2)
  fprintf("Binárne hodnoty sú dotožné.\n");
endif

figure_2 = figure('Visible','off');

hold('on');

title('Exercise 2');
% Vypocitame si 20ms na sekundy
plot_max_2 = double(cut_20ms)/vzorkovacia_frekvencia;

% Zistime si step ktory potrebujeme pouzit pre signal
plot_step_2 = plot_max_2 / length(signal(1:cut_20ms));

% Zistime si step ktory potrebujeme pouzit pre data vygenerovane zo signalu
stem_step_2 = plot_max_2 / length(signal_data_2(1:cut_20ms/16));


% Nakreslime signal od 0ms po 20ms
plot([0:plot_step_2:plot_max_2 - plot_step_2] ,signal(1:cut_20ms));

% Nastavime labely
ylabel("s[n],symbols");
xlabel("t");

% Nakreslime vygenerovane signal data od 0ms po 20ms
stem([8*plot_step_2:stem_step_2:plot_max_2 + 7 * plot_step_2],signal_data_2(1:cut_20ms/16));

% Nastavime velkost okna
axis([0 plot_max_2 -0.8 1]);

% Vykreslime
hold('off');

print(figure_2,'-depsc','2.eps');
close(figure_2);

fprintf('----------------------------------------------------------------------------------------------------------\n');
% -----------------------------------------------------------------------------------------------------------------------

printf('3)\n');

% Nacitame si matice
B_3 = [0.0192 -0.0185 -0.0185 0.0192];
A_3 = [1.0000 -2.8870 2.7997 -0.9113];

% Zistime stabilitu, mozme pouzit aj funkciu isstable
if abs(roots(A_3)) < 1
  printf('Filter je stabilný.\n');
else
  printf('Filter nie je stabilný.\n');  
endif

% Vykreslime
figure_3 = figure('Visible','off');
zplane(B_3,A_3);
title('Exercise 3');
xlabel('Real part');
ylabel('Imaginary part');

% Ulozime do suboru
print(figure_3,'-depsc','3.eps');
close(figure_3);

fprintf('----------------------------------------------------------------------------------------------------------\n');
% -----------------------------------------------------------------------------------------------------------------------

printf('4)\n');

figure_4 = figure('Visible','off');

% 
x_4 = (0 : 499) / 500 * vzorkovacia_frekvencia / 2;
[h_4,w_4] = freqz(B_3,A_3,500);

[minwal_4,column_4] = min(min(h_4(1:125),[],2));

hold('on');

plot(x_4,abs(h_4));

title('Exercise 4');

% Pridame labely
xlabel('f [Hz]');
ylabel('|H(f)|');
legend('Frequency characteristics');

% Pridame grid
grid;

% Nastavime okno
axis([0 8000 0 1]);

hold('off');

% Ulozime do suboru
print(figure_4,'-depsc','4.eps');
close(figure_4);

printf('Filter je typu dolná priepusť.\n');
printf('Mezná frequencia je %f [Hz]\n',x_4(column_4));

fprintf('----------------------------------------------------------------------------------------------------------\n');
% -----------------------------------------------------------------------------------------------------------------------

printf('5\n');

% Vyfiltrujeme signal zadanou differencialnou funkciou
filtered_signal_5 = filter(B_3,A_3,signal);

printf('Posun od ruky vľavo o %d vzorkov.\n',16);

figure_5 = figure('Visible','off');

hold('on');

title('Exercise 5');

ylabel('s[n],ss[n]');
xlabel('n');

plot([0:1:cut_20ms],signal(1:cut_20ms + 1),"r");
plot([0:1:cut_20ms],filtered_signal_5(1:cut_20ms + 1),"b");
plot([0:1:cut_20ms],filtered_signal_5(17:cut_20ms + 17),"g");

hold('off');

axis([0 320 -0.8 0.8]);

print(figure_5,'-depsc','5.eps');
close(figure_5);

fprintf('----------------------------------------------------------------------------------------------------------\n');

% -----------------------------------------------------------------------------------------------------------------------

printf('6)\n');

% Vyfiltrujeme ten signal
filtered_signal_6 = filter(B_3,A_3,signal);

% Shiftneme signal
shifted_filtered_signal_6 = filtered_signal_6(17:length(filtered_signal_6));

% pridame 16 nul nakoniec
shifted_filtered_signal_6 = [shifted_filtered_signal_6,zeros(1,16)];

% Ziskame 20 binarnych symbolov z posunuteho filtrovaneho signalu
i_6 = 0;
shifted_filtered_bin_data_signal_6 = [];

while i_6 < cut_20ms
  i_6 = i_6 + 8;
  
  if shifted_filtered_signal_6(i_6) < 0
    shifted_filtered_bin_data_signal_6 = [shifted_filtered_bin_data_signal_6,0];
  else
    shifted_filtered_bin_data_signal_6 = [shifted_filtered_bin_data_signal_6,1];
  endif
  i_6 = i_6 + 8;
endwhile

max_6 = double(cut_20ms) / vzorkovacia_frekvencia;
step_plot_6 = max_6 / double(cut_20ms);
step_stem_6 = max_6 / double(length(shifted_filtered_bin_data_signal_6));

figure_6 = figure('Visible','off');

title('Exercise 6');

hold('on');

xlabel('t');
ylabel('s[n],ss[n],ssShifted[n],symbols');

% vykreslime origo signal
plot([0:step_plot_6:max_6 - step_plot_6],signal(1:cut_20ms),"b");

% vykreslime vyfiltrovany signal
plot([0:step_plot_6:max_6 - step_plot_6],filtered_signal_6(1:cut_20ms),'color', [0.8500, 0.3250, 0.0980]);

% Vykreslime vyfiltrovany posunuty o 16 dolava signal
plot([0:step_plot_6:max_6 - step_plot_6],shifted_filtered_signal_6(1:cut_20ms),'color', [0.9290, 0.6940, 0.1250]);

% Vykreslime binarne hodnoty posunuteho vyfiltrovaneho signalu
stem([8 * step_plot_6:step_stem_6:max_6 + 7 * step_plot_6],shifted_filtered_bin_data_signal_6,"m");

hold('off');

print(figure_6,'-depsc','6.eps');
close(figure_6);


fprintf('----------------------------------------------------------------------------------------------------------\n');


% -----------------------------------------------------------------------------------------------------------------------
fprintf('7)\n');


filtered_7 = shifted_filtered_signal_6(1:length(shifted_filtered_signal_6) - 16);
filtered_bin_7 = [];
origo_bin_7 = [];

i_7 = 0;
while i_7 < length(shifted_filtered_signal_6) - 16
  i_7 = i_7 + 8;
  
  if filtered_7(i_7) < 0
    filtered_bin_7 = [filtered_bin_7,0];
  else
    filtered_bin_7 = [filtered_bin_7,1];
  endif
  
  if signal(i_7) < 0
    origo_bin_7 = [origo_bin_7,0];
  else
    origo_bin_7 = [origo_bin_7,1];
  endif
  
  i_7 = i_7 + 8;
endwhile

nepodobnost_7 = 0;


for i_7 = 1:length(filtered_bin_7)
  if filtered_bin_7(i_7) != origo_bin_7(i_7)
    nepodobnost_7 = nepodobnost_7 + 1;
  endif
endfor


fprintf('Chybovosť signálov je %f %% a počet chýb je %d\n',(nepodobnost_7/length(filtered_bin_7)) * 100,nepodobnost_7);

fprintf('----------------------------------------------------------------------------------------------------------\n');

% -----------------------------------------------------------------------------------------------------------------------

fprintf('8)\n');

% Ziskame frequenciu
frequency_8 = (0 : vzorkovacia_frekvencia / 2 - 1);

% Ziskame signal
signal_8 = signal(1:vzorkovacia_frekvencia);

% Vyfiltrujeme ho
filtered_signal_8 = filter(B_3,A_3,signal_8);

% Pouzijeme nanho fourierovu funkciu ktoru dame do absolutnej hodnoty
furier_filtered_signal_8 = abs(fft(filtered_signal_8));

furier_signal_8 = abs(fft(signal_8));

% Mozme zacat vykreslovat
figure_8 = figure('Visible','off');

hold('on');

plot(frequency_8,furier_signal_8(1:vzorkovacia_frekvencia/2));
plot(frequency_8,furier_filtered_signal_8(1:vzorkovacia_frekvencia/2));


legend("s[n]","ss[n]");

title('Exercise 8');

grid;
xlabel('f [Hz]');


hold('off');

print(figure_8, '-depsc', '8.eps');
close(figure_8);

fprintf('----------------------------------------------------------------------------------------------------------\n');

% -----------------------------------------------------------------------------------------------------------------------

printf('9)\n');

gmin_9 = min(min(signal));
gmax_9 = max(max(signal));

g_9 = linspace(gmin_9,gmax_9,50);

deltag_9 = abs(g_9(2) - g_9(1));

p_9 = hist(signal,g_9) / length(signal) / deltag_9;


figure_9 = figure('Visible','off');
hold('on');
title('Exercise 9');
xlabel("s[n]");
ylabel("Probability");


plot(g_9,p_9);

axis([ -0.8 0.8 -0.1 1.1 ]);
hold('off');


print(figure_9, '-depsc', '9.eps');
close(figure_9);

printf("Integrál: %f\n",sum(p_9 * deltag_9));

fprintf('----------------------------------------------------------------------------------------------------------\n');

% -----------------------------------------------------------------------------------------------------------------------

printf('10)\n');

% Vypocitame R pomocou funkcie xcorr
R = xcorr(signal) / length(signal);


R = R((-50:50) + length(signal));

figure_10 = figure('Visible', 'off');
%Vykreslime
plot((-50:50), R);

title('Exercise 10');
xlabel('k');
legend('Autocorrelation coefficient');

%vytvorime grid
grid;

% Nastavime okno vypisu
axis([ -51 51 -0.025 0.28]);

print(figure_10, '-depsc', '10.eps');
close(figure_10);

fprintf('----------------------------------------------------------------------------------------------------------\n');
% -----------------------------------------------------------------------------------------------------------------------

fprintf('11)\n');

fprintf('Hodnota koeficientu R[0] : %f\n',R(51));
fprintf('Hodnota koeficientu R[1] : %f\n',R(52));
fprintf('Hodnota koeficientu R[16] : %f\n',R(67));

fprintf('----------------------------------------------------------------------------------------------------------\n');
% -----------------------------------------------------------------------------------------------------------------------

fprintf('\n12,13)\n');

x_12 = linspace(min(signal),max(signal),50);

[h_12,p_12,r_12] = hist2(signal(1:length(signal)-1),signal(2:length(signal)),x_12);

figure_12 = figure('Visible','off');

imagesc(x_12,x_12,p_12);

axis xy;
colorbar;
xlabel('x2');
ylabel('x1');

print(figure_12, '-depsc', '12.eps');
close(figure_12);

fprintf('----------------------------------------------------------------------------------------------------------\n');
% -----------------------------------------------------------------------------------------------------------------------

fprintf('14)\n');

%%% prob dens: will have to normalize
surf = (x_12(2) - x_12(1))^2; % surface of one tile

%%%% autocor coeff 
% make col vector out of x and clone it L times. 
x = x_12(:); 
X1 = repmat(x,1,50);
% make row vector out of x and clone it L times. 
x = x_12(:)'; 
X2 = repmat(x,50,1); 
% now put it together, don't forget to multipl by tile surface
res_12 = sum(sum (X1 .* X2 .* p_12)) * surf;


fprintf('Korelačný koeficient R[1]: %f\n',res_12);
fprintf('Korelačný koeficient R[1] z prikladu 11: %f\n',R(52));

fprintf('----------------------------------------------------------------------------------------------------------\n');