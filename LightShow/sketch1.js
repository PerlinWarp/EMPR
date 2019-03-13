var mic, cnv, speed;
var button, micOn, fft;

var volHistory = [];
var w; //Width of each band. 

function setup() {
  cnv = createCanvas(256, 256);
  speed = 1;
  //colorMode(HSB);
  //Microphone setup
  mic = new p5.AudioIn();
  mic.start();
  micOn = false;
  fft = new p5.FFT(0, 1024);
  fft.setInput(mic);
  w = width / 1024;

  button = createButton('toggle');
  button.mousePressed(toggleMic);
}

function draw() {
	if(micOn){
		plot();
	}
}

function plot(){
  background(220);
	copy(cnv,0,0,width,height,0,speed,width,height);
	
	var vol = mic.getLevel();
	var spectrum = fft.analyze();

  noFill();
  stroke(255);

	beginShape();
  vertex(0,height)
	for (var i = 0; i < spectrum.length; i++){
		var amp = spectrum[i];
    var x = map(i, 0, 1024, 0, 256);
		var y = map(amp, 0, 256, height, 0);
		colour = map(i, 0,spectrum.length, 0, 255);
		//fill(colour,40,255);
		vertex(i*w, y);
	}
  vertex(width,height)
	endShape();

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
