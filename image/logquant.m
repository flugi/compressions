function b = logquant(a, q)
  sg = sign(a);
  a=exp((round(log(abs(double(a))+1)/q)*q))-1;
    b = sg.*a;
end
