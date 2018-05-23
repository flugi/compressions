function y=mdct(x)
% MDCT      Compute MDCT through FFT
%
%       Synopsys:
%           y = mdct(x)
%
%       Parameters:
%           x = Input data
%
%       Description:
%           MDCT is the most widely used transform in audio coding, such as
%           MP3, AAC, OGG Vorbis, WMA. see
%           http://en.wikipedia.org/wiki/Modified_discrete_cosine_transform
%           for more information.          
% 
%           Suppose N=size(x,1)/2. The MDCT transform matrix is
%               C=cos(pi/N*([0:2*N-1]'+.5+.5*N)*([0:N-1]+.5));
%           then MDCT spectrum is
%               y=C'*x;
%
%           The fast algorithm used here consists of 4 steps:    
%               (1) fold column-wise 2*N rows into N rows 
%               (2) complex arrange the N rows into N/2 rows
%               (3) pre-twiddle, N/2-CFFT, post-twiddle
%               (4) reorder to form the MDCT spectrum     
%           In fact, (2)-(4) is a fast DCT-IV algorithm.             
%
%       Assumption:
%           (1) x is real  
%           (2) size(x,1) is a multiple of 4
%              


nr=size(x,1);

% column oriented operation
if(nr==1)
    x=x';
end

if(mod(size(x,1),4)~=0)
   error(" Expecting MDCT length a multiple of 4");    
end

N=size(x,1)/2;

persistent old_size w

if(isempty(old_size) || old_size~=N)
    % twiddle factors
    w=sparse(1:N/2, 1:N/2, exp(-j*2*pi/(2*N)*([0:N/2-1]'+.125))); 
    w=N^-.25*w;
	old_size=N;
end

% oddly fold the first N rows
xp(1:N/2,:)=x(N/2+1:N,:)-x(N/2:-1:1,:);
% evenly fold the last N rows
xp(N/2+1:N,:)=x(N+1:3*N/2,:)+x(2*N:-1:3*N/2+1,:);

% complex arrange into N/2 rows
xc=xp(1:2:N,:)-j*xp(N:-2:1,:);

% pre-twiddle, N/2-CFFT, and post-twiddle
yc=w*fft(w*xc);

% map to MDCT
y(1:2:N,:)=imag(yc(1:N/2,:));
y(2:2:N,:)=real(yc(N/2:-1:1,:));

% row input, row output
if(nr==1)
    y=y';
end

