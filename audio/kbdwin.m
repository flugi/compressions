function y=kbdwin(n, alpha)
% KBDWIN    Generate Kaiser-Bessel-Derived Window 
%         
%       Synopsys:
%           y = kbdwin(n, alpha)
%
%       Parameters:
%           n     = KBD window length 
%           alpha = shape parameter, larger for higher sideband attenuation
% 
%       Description:
%           Window function used in AAC, see 
%           http://en.wikipedia.org/wiki/Kaiser_window for more information 
%
%       Assumption:
%           (1) n is a even number
%           (2) alpha is real
%  


% Modified Bessel function of the first kind, 0th order
x=pi*alpha*(1-(4*[0:n/2]'/n-1).^2).^.5;
x=besseli(0,x);

% accumulation
y=zeros(n,1);
y(1:n/2+1)=cumsum(x);

% normalization and mirror
y(1:n/2)=((y(1:n/2)/y(n/2+1))).^.5;
y(n/2+1:n)=y(n/2:-1:1);

