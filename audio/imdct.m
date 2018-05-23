function x=imdct(y)
% IMDCT     Compute IMDCT through FFT
%
%       Synopsys:
%           x = imdct(y)
%
%       Parameters:
%           y = Input data
%
%       Description:
%           MDCT is the most widely used transform in audio coding, such as
%           MP3, AAC, OGG Vorbis, WMA. see
%           http://en.wikipedia.org/wiki/Modified_discrete_cosine_transform
%           for more information.          
%
%           Suppose N=size(y,1). The MDCT transform matrix is
%               C=cos(pi/N*([0:2*N-1]'+.5+.5*N)*([0:N-1]+.5));
%           then given a MDCT spectrum y, the time signal x is
%               x=C*y;
%
%           The fast algorithm used here consists of 4 steps:    
%               (1) complex arrange N rows into N/2 rows
%               (2) pre-twiddle, N/2-CIFFT, post-twiddle
%               (3) map to N real rows      
%               (4) unfold the N rows into 2*N rows 
%           This is the inverse of the fast MDCT algorithm
%
%       Assumption:
%           (1) y is real  
%           (2) size(y,1) is a multiple of 2
%              


nr=size(y,1);

% column oriented operation
if(nr==1)
    y=y';
end

if(mod(size(y,1),2)~=0)
   error(" Expecting IMDCT length a multiple of 2");    
end

N=size(y,1);

persistent old_size w

if(isempty(old_size) || old_size~=N)
    % twiddle factors
    w=sparse(1:N/2, 1:N/2, exp(j*2*pi/(2*N)*([0:N/2-1]'+.125))); 
    w=N^.25*w;
	old_size=N;
end

% complex arrange into N/2 rows
yc=y(N:-2:2,:)+j*y(1:2:N,:); 

% pre-twiddle, N/2-CIFFT, and post-twiddle
xc=w*ifft(w*yc);

% map to N rows
xp(1:2:N,:)=real(xc);
xp(N:-2:1,:)=-imag(xc);

% odd unfold to first N rows
x(1:N/2,:)=-xp(N/2:-1:1);
x(N/2+1:N,:)=xp(1:N/2);
% even unfold to last N rows
x(N+1:3*N/2,:)=xp(N/2+1:N,:);
x(3*N/2+1:2*N,:)=xp(N:-1:N/2+1,:);

% row input, row output
if(nr==1)
    x=x';
end

