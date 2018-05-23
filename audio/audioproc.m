function [ md  resynth ] = audioproc(w)
  s = size(w,1)
  window = 1024;
  step = window/2;
  ns = floor((s-window)/step)*step;
  N = floor((ns-window)/step)
  w1=w(1:ns);
%  x = reshape(w1,step, ns/step);
  mi = repmat(1:window,[N,1]) + repmat((step) * (0:(N-1))',[1,window]);
  x = w1(mi)';
  x = x .* kbdwin(window,1);
  md = mdct(x);
  quant = 0.4;
  md = round(md/quant)*quant;
  z1 = x;
  test = imdct(md(:,1));
  for i = 1:N
    z1(:,i) = imdct(md(:,i));
  endfor
  z1 = z1 .* kbdwin(window,1);
%  resynth = reshape(z1,ns, 1);
  resyn = zeros(ns,1);
  whos
  for i=1:N
    resyn((i-1)*step+1:(i-1)*step+window) += z1(:,i); 
  endfor
  resynth = resyn;
endfunction
