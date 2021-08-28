W,H = GetImageSize();

ResizeCanvas( W + 2, H + 2, true, true );

-- copy top line
for j = 0, W - 1 do
  PutPixelRGB( j + 1, 0, GetPixelRGB( j + 1, 1 ) );  
end
-- copy bottom line
for j = 0, W - 1 do
  PutPixelRGB( j + 1, H + 1, GetPixelRGB( j + 1, H ) );  
end

-- copy left side
for j = 0, H + 1 do
  PutPixelRGB( 0, j, GetPixelRGB( 1, j ) );  
end

-- copy right side
for j = 0, H + 1 do
  PutPixelRGB( W + 1, j, GetPixelRGB( W, j ) );  
end
