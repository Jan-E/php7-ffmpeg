<?php
extension_loaded('ffmpeg') or die('Error in loading ffmpeg');
$movie = isset($_GET['movie']) ? urldecode($_GET['movie']) : 'sample.mp4';
$height = isset($_GET['height']) ? stripslashes($_GET['height']) : 0;
$width = isset($_GET['width']) ? stripslashes($_GET['width']) : 240;
$frame = isset($_GET['frame']) ? stripslashes($_GET['frame']) : 5;

if (!file_exists($movie)) die ("$movie: unknown movie");
$ffmpegInstance = new ffmpeg_movie($movie);
$ffmpegFrame = $ffmpegInstance->getFrame($frame);
$ffmpegImage = $ffmpegFrame->toGDImage();
$par = $ffmpegInstance->getPixelAspectRatio();
if (!$par) $par = 1;
$width = $ffmpegInstance->getFrameWidth();
$height = $ffmpegInstance->getFrameHeight();
$destwidth = $width;
while ($destwidth > 384) {
  $destwidth = round($destwidth / 2);
}
$destheight = round($height * $destwidth / $width / $par);

$ffmpegDest = imagecreatetruecolor($destwidth,$destheight);
imagecopyresized($ffmpegDest, $ffmpegImage, 0, 0, 0, 0, $destwidth, $destheight, $width, $height);
imagedestroy($ffmpegImage);

header("Pragma: no-cache");
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Cache-control: no-cache, no-store, must-revalidate, max_age=0");
header("Content-type: image/png");
imagepng($ffmpegDest);
imagedestroy($ffmpegDest);
?>
