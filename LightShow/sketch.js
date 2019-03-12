var mic;
var button, micOn, fft;

var volHistory = [];

function setup() {
  createCanvas(256, 256);

  //Microphone setup
  mic = new p5.AudioIn();
  mic.start();
  micOn = true;
  fft = new p5.FFT(0, 256);
  fft.setInput(mic);

  button = createButton('toggle');
  button.mousePressed(toggleMic);
}

function draw() {
	background(0);
	if(micOn){
		var vol = mic.getLevel();
		stroke(255)
		var spectrum = fft.analyze();
		for (var i = 0; i < spectrum.length; i++){
			var amp = spectrum[i];
			var y = map(amp, 0, 256, height, 0);
			line(i, height, i, y);
		}
	}
}

function toggleMic(){
	if (micOn){
		mic.stop();
		micOn = false;
	}else{
		mic.start();
		micOn = true;
	}

}

// Needed to get the audio working on chrome
function touchStarted() { getAudioContext().resume(); } 
