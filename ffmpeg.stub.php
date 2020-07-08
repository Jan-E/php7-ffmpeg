<?php

/** @generate-function-entries */

/*
 usage example
 
 $ffmpegInstance = new ffmpeg_movie($filename);
 $width = $ffmpegInstance->getFrameWidth();
 $height = $ffmpegInstance->getFrameHeight();
 $ffmpegFrame = $ffmpegInstance->getFrame($int);
 $ffmpegImage = $ffmpegFrame->toGDImage();
 */

class ffmpeg_movie {

	public function __construct(string $filename) {}

	public function getframewidth(): int {}

	public function getframeheight(): int {}

//	public function getframe($int) {}

}

class ffmpeg_frame {
	// togdimage is only active when HAVE_LIBGD20
//	public function togdimage(): int {}
	
	public function getwidth(): int {}
	
	public function getheight(): int {}

	public function iskeyframe(): bool {}

}
