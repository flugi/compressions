function sig = lpcresynth(pc, start, length, residual)
  res = zeros(length,1);
  n = size(start,1);
  res(1:n)=start(1:n);
  for i= n+1 : length
    res(i)=res(i-n:i-1)' * -flip(pc(2:n+1))' + residual(i);
  endfor
  sig = res;
endfunction
