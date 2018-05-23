%pkg: signal nan tsa

function resynth = speechcoding(w)
  s = size(w,1);
  window = 1024;
  step = window/2;
  ns = floor((s-window)/step)*step;
  N = floor((ns-window)/step);
  w1=w(1:ns);
  resyn = zeros(ns,1);
  mi = repmat(1:window,[N,1]) + repmat((step) * (0:(N-1))',[1,window]);
  x = w1(mi)';
  energ = sum(abs(x));
  for i=1:N
    printf("%d / %d \n",i,N); fflush(stdout);
    a = lpc(x(:,i),6);
    x_1=filter(-a(2:7),1,x(:,i));
    residual = x_1-x(:,i);
    mx = max(abs(residual));
    qresidual_dirac = mx*sign(residual).*(abs(residual<0.9*mx));
    qresidual_logscale = exp(round(log(abs(residual)))).*sign(residual);
    x_rs = lpcresynth(a,x(1:6,i),window,qresidual_dirac);
    ratio=energ(i) / sum(abs(x_rs));
    x_rs = x_rs*ratio;
    z1 = x_rs;% .* kbdwin(window,1);
    resyn((i-1)*step+1:(i-1)*step+window) += z1; 
  endfor
  resynth = resyn;

endfunction
