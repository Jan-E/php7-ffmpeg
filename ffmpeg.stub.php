<?php

/** @generate-function-entries */

/*
 usage example
 
 $ffmpegInstance = new ffmpeg_movie($filename);
 $movie_width = $ffmpegInstance->getFrameWidth();
 $movie_height = $ffmpegInstance->getFrameHeight();

 $ffmpegFrame = $ffmpegInstance->getFrame(10);
 $frame_width = $ffmpegFrame->getWidth();
 $frame_height = $ffmpegFrame->getWidth();

 $ffmpegImage = $ffmpegFrame->toGDImage();
 */

class ffmpeg_movie {

	public function __construct(string $filename) {}

	public function getduration(): double {}
	public function getframecount(): int {}
	public function getframerate(): double {}
	public function getfilename(): string {}
	public function getcomment(): string {}
	public function gettitle(): string {}
	public function getauthor(): string {}
	public function getartist(): string {}
	public function getcopyright(): string {}
	public function getalbum(): string {}
	public function getgenre(): string {}
	public function getyear(): int {}
	public function gettracknumber(): int {}
	public function getframewidth(): int {}
	public function getframeheight(): int {}
	public function getframenumber(): int {}
	public function getpixelformat(): string {}
	public function getbitrate(): int {}
	public function hasaudio(): bool {}
	public function hasvideo(): bool {}
	public function getnextkeyframe(): ffmpeg_frame {}
	public function getframe($int): ffmpeg_frame {}
	public function getvideocodec(): string {}
	public function getaudiocodec(): string {}
	public function getvideostreamid(): int {}
	public function getaudiostreamid(): int {}
	public function getaudiochannels(): int {}
	public function getaudiosamplerate(): int {}
	public function getaudiobitrate(): int {}
	public function getvideobitrate(): int {}
	public function getpixelaspectratio(): double {}

}

class ffmpeg_frame {
	// togdimage is only active when HAVE_LIBGD20
	public function togdimage(): GdImage|false {}
	
	public function getwidth(): int {}
	public function getheight(): int {}
	public function resize(int $wanted_width, ?int $wanted_height = null, ?int $crop_top = null, ?int $crop_bottom = null, ?int $crop_left = null, ?int $crop_right = null): bool {}
	public function iskeyframe(): bool {}
	public function getpresentationtimestamp(): double {}
	public function getpts(): double {}

}
