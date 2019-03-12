var mic;
var button, micOn, fft;

var volHistory = [];
var w; //Width of each band. 

function setup() {
  createCanvas(256, 256);
  //colorMode(HSB);
  //Microphone setup
  mic = new p5.AudioIn();
  mic.start();
  micOn = false;
  fft = new p5.FFT(0, 1024);
  fft.setInput(mic);
  w = width / 64;

  button = createButton('toggle');
  button.mousePressed(toggleMic);
}

function draw() {
	background(0);
	if(micOn){
		plot();
	}
}

function plot(){
  var waveform = fft.waveform();
  noFill();
  beginShape();
  stroke(255,0,0); // waveform is red
  strokeWeight(1);
  for (var i = 0; i< waveform.length; i++){
    var x = map(i, 0, waveform.length, 0, width);
    var y = map( waveform[i], -1, 1, 0, height);
    vertex(x,y);
  }
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
