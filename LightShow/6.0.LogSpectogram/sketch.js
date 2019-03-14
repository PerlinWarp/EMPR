var mic,fft;

// Our canvas object, used for copying
var cnv;
var len;

// How fast do we want to move the canvas left
var speed = 10;

function setup() {
  cnv = createCanvas(windowWidth, windowHeight);
  colorMode(HSB);
  background(0);
  noStroke();
  // Get input from the mic
  mic = new p5.AudioIn();
  mic.start();
  fft = new p5.FFT(0,1024);
  fft.setInput(mic);

  // Fixing the permissions issue on chrome
  touchStarted();

  len = windowHeight / 1024;
}

function draw() {
  //Copy the last part of the window and move it left
  copy(cnv, 0, 0, width, height, -speed, 0, width, height);

  var spectrum = fft.analyze();
  console.log(spectrum);

  for (var i = 0; i < spectrum.length; i++){
    var index = logScale(i, spectrum.length);
    fill(spectrum[index], 255, spectrum[index]);
    var percent = i / spectrum.length;
    var y = percent * height;
    rect(width - speed, height - y, speed, height / spectrum.length);
    //rect(width - speed, height - (i*len), 100, height - ((i+1)*len));
  }
}

// Needed to get the audio working on chrome
function touchStarted() { getAudioContext().resume(); } 

//We want to try to stretch the graph logarithmically 
function logScale(index, total) {
  var logmax = Math.log2(total + 1);
  var exp = logmax * index / total;
  return Math.round(Math.pow(2, exp) - 1);
}