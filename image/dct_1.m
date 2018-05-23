function dct_1(x=1, y=1)
%  [img, map, alpha]=imread('./windows_xp_bliss-wide.jpg' );
  [img, map, alpha]=imread('./macska.png' );
  subimg=img(1+x:32+x, 1+y:32+y, :);
  a=subimg(:,:,2);
  d=dct2(a);
  id = idct2(d);
  d1 = round(d/10)*10;
  id1=idct2(d1);
  d2 = round(d/50)*50;
  id2=idct2(d2);
  figure
  colormap(gray)
  subplot(2,3,1);
  image(a/4)
  subplot(2,3,2);
  image(id/4)
  subplot(2,3,3);
  image(d)
  subplot(2,3,4);
  image(id1/4);
  subplot(2,3,5);
  image(id2/4);
  subplot(2,3,6);
  image(d2)
end
