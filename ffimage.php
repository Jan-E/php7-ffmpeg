<?php
extension_loaded('ffmpeg') or die('Error in loading ffmpeg');
$movie = isset($_REQUEST['movie']) ? urldecode($_REQUEST['movie']) : 'sample.mp4';
$height = isset($_REQUEST['height']) ? stripslashes($_REQUEST['height']) : 270;
$width = isset($_REQUEST['width']) ? stripslashes($_REQUEST['width']) : 480;
$frame = isset($_REQUEST['frame']) ? stripslashes($_REQUEST['frame']) : 5;

if (!file_exists($movie)) die ("$movie: unknown movie");
$ffmpegInstance = new ffmpeg_movie($movie);
$ffmpegFrame = $ffmpegInstance->getFrame($frame);
$ffmpegImage = $ffmpegFrame->toGDImage();
$par = $ffmpegInstance->getPixelAspectRatio();
if (!$par) $par = 1;
$width = $ffmpegInstance->getFrameWidth();
$height = $ffmpegInstance->getFrameHeight();
$destwidth = $width;
while ($destwidth > 480) {
  $destwidth = 2 * round($destwidth / 4);
}
$destheight = 2 * round($height * $destwidth / $width / $par / 2);

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
