W,H = GetImageSize();

TileX = tonumber( InputValue( "Width of Cut", 20 ) );
TileY = tonumber( InputValue( "Height of Cut", 20 ) );

if ( ( TileX <= 0 )
or   ( TileY <= 0 ) ) then
  return;
end

TileCountX = math.floor( W / TileX );
TileCountY = math.floor( H / TileY );

for j = 0, TileCountY do
  for i = 0, TileCountX do
    local newImageWidth = TileX;
    local newImageHeight = TileY;
    if ( i * TileX + newImageWidth > W ) then
      newImageWidth = W - i * TileX;
    end
    if ( j * TileY + newImageHeight > H ) then
      newImageHeight = H - j * TileY;
    end
    if ( ( newImageWidth > 0 )
    and  ( newImageHeight > 0 ) ) then
      CreateNewImageFromRect( i * TileX, j * TileY, newImageWidth, newImageHeight );
    end
  end
end