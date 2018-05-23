function result = dct_image(bs, quality)
%  img = imread('./windows_xp_bliss-wide.jpg');
  img = imread('./macska.png');
  [sx sy channels] = size(img)
  nsx = floor(sx/bs)*bs
  nsy = floor(sy/bs)*bs
  img = img(1:nsx, 1:nsy,:);
  rimg= img(:,:,1);
  gimg= img(:,:,2);
  bimg= img(:,:,3);
  drimg = process(rimg, bs, quality);
  dgimg = process(gimg, bs, quality);
  dbimg = process(bimg, bs, quality);
  result = zeros(nsx, nsy, channels);
  result(1:nsx,1:nsy,1)=drimg;
  result(1:nsx,1:nsy,2)=dgimg;
  result(1:nsx,1:nsy,3)=dbimg;
  result = uint8(result);
end

function dimg = process(img, bs, quality)
  sum_un = 0;
  [sx sy channels] = size(img);
  res = img;
  for x = 1:sx/bs-1
    for y = 1:sy/bs-1
      [ encblock un] = encodeblock(img, bs,x,y,quality); 
      res(x*bs+1:(x+1)*bs, y*bs+1:(y+1)*bs) = encblock;
      sum_un += un;
    endfor
  endfor
%  figure
%  image(img/4)
%  figure
%  image(res/4)
  printf("sum of unique values in blocks: %d  avg: %f\n",sum_un, sum_un/((sx/bs-1)*(sy/bs-1)));
  dimg = uint8(res);  
endfunction

function [ encblock, un] = encodeblock(img, bs, x, y, quality)
  subimg=img(x*bs+1:(x+1)*bs, y*bs+1:(y+1)*bs);
  d=dct2(subimg);
  d1 = round(d/quality)*quality;
%  d1 = logquant(d, quality);
%  zigzag(uint32(d1))
%  zigzag(uint32(d1))
  un = size(unique(d1),1);
  encblock=idct2(d1);
endfunction
