W,H = GetImageSize();

tileWidth = 8
tileHeight = 8

for y = 0, H / tileHeight do
  for x = 0, W / tileWidth do
    local tileX = x;
    local tileY = y;
    
    local img = GetImage( x * tileWidth, y * tileHeight, tileWidth, tileHeight );
    
    local pixel = img:GetPixelRGB( 0, 0 );
    
    _ALERT( "Pixel: ", pixel );
  end
end

